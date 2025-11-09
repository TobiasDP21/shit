using System;
using System.IO;
using System.IO.Pipes;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace UnityReflectionMod
{
    public class IPCServer
    {
        private const string PipeName = "UnityReflectionPipe";
        private NamedPipeServerStream? pipeServer;
        private bool isRunning;
        private Thread? serverThread;
        private int updateIntervalMs = 1000; // Update every 1 second by default

        public event Action<string>? OnLog;
        public event Action<string>? OnError;

        public int UpdateInterval
        {
            get => updateIntervalMs;
            set => updateIntervalMs = Math.Max(100, value); // Minimum 100ms
        }

        public void Start()
        {
            if (isRunning)
            {
                Log("Server already running");
                return;
            }

            isRunning = true;
            serverThread = new Thread(ServerLoop)
            {
                IsBackground = true,
                Name = "IPC Server Thread"
            };
            serverThread.Start();
            Log($"IPC Server started on pipe: {PipeName}");
        }

        public void Stop()
        {
            isRunning = false;

            try
            {
                pipeServer?.Dispose();
            }
            catch { }

            serverThread?.Join(1000);
            Log("IPC Server stopped");
        }

        private void ServerLoop()
        {
            bool useBidirectional = true;

            while (isRunning)
            {
                try
                {
                    Log("Creating named pipe server...");

                    // Try bidirectional first, fall back to write-only if not supported
                    PipeDirection direction = useBidirectional ? PipeDirection.InOut : PipeDirection.Out;

                    using (pipeServer = new NamedPipeServerStream(
                        PipeName,
                        direction,
                        1,
                        PipeTransmissionMode.Byte,
                        PipeOptions.None)) // Use synchronous mode for Mono compatibility
                    {
                        if (useBidirectional)
                        {
                            Log("Bidirectional pipe created (commands supported)");
                        }
                        else
                        {
                            Log("Write-only pipe created (commands not supported on this platform)");
                        }
                        Log("Waiting for client connection...");

                        try
                        {
                            pipeServer.WaitForConnection();
                        }
                        catch (NotImplementedException ex)
                        {
                            LogError($"WaitForConnection not supported: {ex.Message}");
                            LogError("Attempting synchronous connection...");
                            // Platform might not support async, try without waiting
                        }

                        if (!isRunning) break;

                        Log("Client connected! Starting live data stream...");

                        // Start command reader thread only if bidirectional
                        if (useBidirectional)
                        {
                            try
                            {
                                var commandThread = new Thread(() => ReadCommands(pipeServer))
                                {
                                    IsBackground = true,
                                    Name = "Command Reader Thread"
                                };
                                commandThread.Start();
                                Log("Command reader thread started");
                            }
                            catch (Exception ex)
                            {
                                LogError($"Could not start command reader: {ex.Message}");
                            }
                        }

                        // Send live updates
                        while (isRunning && pipeServer.IsConnected)
                        {
                            try
                            {
                                // Get fresh reflection data
                                var data = AssemblyReflector.ReflectAssemblyCSharp();

                                // Serialize to JSON
                                string json = SerializeToJson(data);

                                // Send data with length prefix
                                byte[] buffer = Encoding.UTF8.GetBytes(json);
                                byte[] lengthPrefix = BitConverter.GetBytes(buffer.Length);

                                pipeServer.Write(lengthPrefix, 0, lengthPrefix.Length);
                                pipeServer.Write(buffer, 0, buffer.Length);
                                pipeServer.Flush();

                                Log($"Sent update: {buffer.Length} bytes ({data.Types.Count} types)");

                                // Wait before next update
                                Thread.Sleep(updateIntervalMs);
                            }
                            catch (IOException)
                            {
                                Log("Client disconnected");
                                break;
                            }
                            catch (Exception ex)
                            {
                                LogError($"Error sending data: {ex.Message}");
                                break;
                            }
                        }

                        Log("Client session ended");
                    }
                }
                catch (NotImplementedException ex)
                {
                    if (useBidirectional)
                    {
                        LogError($"Bidirectional pipes not supported: {ex.Message}");
                        LogError("Falling back to write-only mode (commands will not work)");
                        useBidirectional = false;
                        // Retry immediately with write-only mode
                        continue;
                    }
                    else
                    {
                        LogError($"Fatal error - basic pipe operations not supported: {ex.Message}");
                        break;
                    }
                }
                catch (Exception ex)
                {
                    if (isRunning)
                    {
                        // Check if it's a "not implemented" error (might be wrapped)
                        if (ex.Message.Contains("not implemented") && useBidirectional)
                        {
                            LogError($"Bidirectional mode failed: {ex.Message}");
                            LogError("Switching to write-only mode...");
                            useBidirectional = false;
                            continue;
                        }

                        LogError($"Server error: {ex.Message}");
                        Thread.Sleep(1000); // Wait before retry
                    }
                }
            }
        }

        private void ReadCommands(NamedPipeServerStream pipe)
        {
            try
            {
                // Check if the pipe supports reading
                if (!pipe.CanRead)
                {
                    LogError("Pipe does not support reading (platform limitation)");
                    return;
                }

                byte[] buffer = new byte[1024];
                while (isRunning && pipe.IsConnected)
                {
                    try
                    {
                        // Attempt to read with a small timeout approach
                        int bytesRead = pipe.Read(buffer, 0, buffer.Length);
                        if (bytesRead > 0)
                        {
                            string command = Encoding.UTF8.GetString(buffer, 0, bytesRead).Trim();
                            ProcessCommand(command);
                        }
                        else
                        {
                            // No data, wait a bit before trying again
                            Thread.Sleep(100);
                        }
                    }
                    catch (NotImplementedException)
                    {
                        LogError("Read operation not implemented on this platform - command support disabled");
                        return;
                    }
                }
            }
            catch (IOException)
            {
                // Client disconnected, normal exit
                Log("Command reader: client disconnected");
            }
            catch (NotImplementedException)
            {
                LogError("Named pipe read operations not supported on this platform");
            }
            catch (Exception ex)
            {
                LogError($"Error reading command: {ex.Message}");
            }
        }

        private void ProcessCommand(string command)
        {
            Log($"Received command: {command}");

            if (command.StartsWith("INTERVAL:"))
            {
                if (int.TryParse(command.Substring(9), out int interval))
                {
                    UpdateInterval = interval;
                    Log($"Update interval set to {interval}ms");
                }
            }
            else if (command == "REFRESH")
            {
                Log("Manual refresh requested");
                // Next update will happen on next loop iteration
            }
        }

        private string SerializeToJson(AssemblyData data)
        {
            // Simple JSON serialization without dependencies
            var sb = new StringBuilder();
            sb.Append("{");
            sb.Append($"\"assemblyName\":\"{EscapeJson(data.AssemblyName)}\",");
            sb.Append($"\"timestamp\":\"{data.Timestamp:O}\",");
            sb.Append("\"types\":[");

            for (int i = 0; i < data.Types.Count; i++)
            {
                if (i > 0) sb.Append(",");
                SerializeType(sb, data.Types[i]);
            }

            sb.Append("]}");
            return sb.ToString();
        }

        private void SerializeType(StringBuilder sb, TypeInfo type)
        {
            sb.Append("{");
            sb.Append($"\"name\":\"{EscapeJson(type.Name)}\",");
            sb.Append($"\"fullName\":\"{EscapeJson(type.FullName)}\",");
            sb.Append($"\"namespace\":\"{EscapeJson(type.Namespace)}\",");
            sb.Append($"\"baseType\":\"{EscapeJson(type.BaseType)}\",");
            sb.Append($"\"isClass\":{(type.IsClass ? "true" : "false")},");
            sb.Append($"\"isStruct\":{(type.IsStruct ? "true" : "false")},");
            sb.Append($"\"isEnum\":{(type.IsEnum ? "true" : "false")},");
            sb.Append($"\"isInterface\":{(type.IsInterface ? "true" : "false")},");

            // Fields
            sb.Append("\"fields\":[");
            for (int i = 0; i < type.Fields.Count; i++)
            {
                if (i > 0) sb.Append(",");
                var field = type.Fields[i];
                sb.Append("{");
                sb.Append($"\"name\":\"{EscapeJson(field.Name)}\",");
                sb.Append($"\"fieldType\":\"{EscapeJson(field.FieldType)}\",");
                sb.Append($"\"isPublic\":{(field.IsPublic ? "true" : "false")},");
                sb.Append($"\"isStatic\":{(field.IsStatic ? "true" : "false")},");
                sb.Append($"\"isReadOnly\":{(field.IsReadOnly ? "true" : "false")}");
                sb.Append("}");
            }
            sb.Append("],");

            // Methods
            sb.Append("\"methods\":[");
            for (int i = 0; i < type.Methods.Count; i++)
            {
                if (i > 0) sb.Append(",");
                var method = type.Methods[i];
                sb.Append("{");
                sb.Append($"\"name\":\"{EscapeJson(method.Name)}\",");
                sb.Append($"\"returnType\":\"{EscapeJson(method.ReturnType)}\",");
                sb.Append($"\"isPublic\":{(method.IsPublic ? "true" : "false")},");
                sb.Append($"\"isStatic\":{(method.IsStatic ? "true" : "false")},");
                sb.Append("\"parameters\":[");
                for (int j = 0; j < method.Parameters.Count; j++)
                {
                    if (j > 0) sb.Append(",");
                    var param = method.Parameters[j];
                    sb.Append("{");
                    sb.Append($"\"name\":\"{EscapeJson(param.Name)}\",");
                    sb.Append($"\"parameterType\":\"{EscapeJson(param.ParameterType)}\"");
                    sb.Append("}");
                }
                sb.Append("]}");
            }
            sb.Append("],");

            // Properties
            sb.Append("\"properties\":[");
            for (int i = 0; i < type.Properties.Count; i++)
            {
                if (i > 0) sb.Append(",");
                var prop = type.Properties[i];
                sb.Append("{");
                sb.Append($"\"name\":\"{EscapeJson(prop.Name)}\",");
                sb.Append($"\"propertyType\":\"{EscapeJson(prop.PropertyType)}\",");
                sb.Append($"\"canRead\":{(prop.CanRead ? "true" : "false")},");
                sb.Append($"\"canWrite\":{(prop.CanWrite ? "true" : "false")}");
                sb.Append("}");
            }
            sb.Append("]");

            sb.Append("}");
        }

        private string EscapeJson(string str)
        {
            if (string.IsNullOrEmpty(str)) return string.Empty;
            return str.Replace("\\", "\\\\")
                      .Replace("\"", "\\\"")
                      .Replace("\n", "\\n")
                      .Replace("\r", "\\r")
                      .Replace("\t", "\\t");
        }

        private void Log(string message)
        {
            OnLog?.Invoke(message);
        }

        private void LogError(string message)
        {
            OnError?.Invoke(message);
        }
    }
}
