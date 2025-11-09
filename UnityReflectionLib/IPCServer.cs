using System;
using System.IO;
using System.IO.Pipes;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace UnityReflectionLib
{
    public class IPCServer
    {
        private const string PipeName = "UnityReflectionPipe";
        private NamedPipeServerStream? pipeServer;
        private bool isRunning;
        private Thread? serverThread;

        public event Action<string>? OnLog;
        public event Action<string>? OnError;

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
            while (isRunning)
            {
                try
                {
                    using (pipeServer = new NamedPipeServerStream(
                        PipeName,
                        PipeDirection.Out,
                        1,
                        PipeTransmissionMode.Byte,
                        PipeOptions.Asynchronous))
                    {
                        Log("Waiting for client connection...");

                        pipeServer.WaitForConnection();

                        if (!isRunning) break;

                        Log("Client connected!");

                        // Get reflection data
                        var data = AssemblyReflector.ReflectAssemblyCSharp();

                        // Serialize to JSON
                        string json = SerializeToJson(data);

                        // Send data
                        byte[] buffer = Encoding.UTF8.GetBytes(json);
                        byte[] lengthPrefix = BitConverter.GetBytes(buffer.Length);

                        pipeServer.Write(lengthPrefix, 0, lengthPrefix.Length);
                        pipeServer.Write(buffer, 0, buffer.Length);
                        pipeServer.Flush();

                        Log($"Sent {buffer.Length} bytes to client");

                        Thread.Sleep(500); // Give client time to read
                    }
                }
                catch (Exception ex)
                {
                    if (isRunning)
                    {
                        LogError($"Server error: {ex.Message}");
                        Thread.Sleep(1000); // Wait before retry
                    }
                }
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
            UnityEngine.Debug.Log($"[IPCServer] {message}");
        }

        private void LogError(string message)
        {
            OnError?.Invoke(message);
            UnityEngine.Debug.LogError($"[IPCServer] {message}");
        }
    }
}
