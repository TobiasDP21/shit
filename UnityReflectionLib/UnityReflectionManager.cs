using UnityEngine;

namespace UnityReflectionLib
{
    /// <summary>
    /// Add this component to a GameObject in your Unity scene to enable reflection IPC
    /// </summary>
    public class UnityReflectionManager : MonoBehaviour
    {
        private IPCServer? ipcServer;

        [Header("Settings")]
        [Tooltip("Start the IPC server automatically on Awake")]
        public bool autoStart = true;

        [Header("Status")]
        [SerializeField] private bool isServerRunning = false;

        private void Awake()
        {
            if (autoStart)
            {
                StartServer();
            }
        }

        private void OnDestroy()
        {
            StopServer();
        }

        [ContextMenu("Start IPC Server")]
        public void StartServer()
        {
            if (ipcServer != null)
            {
                Debug.LogWarning("IPC Server already running");
                return;
            }

            ipcServer = new IPCServer();
            ipcServer.OnLog += OnServerLog;
            ipcServer.OnError += OnServerError;
            ipcServer.Start();
            isServerRunning = true;
        }

        [ContextMenu("Stop IPC Server")]
        public void StopServer()
        {
            if (ipcServer != null)
            {
                ipcServer.Stop();
                ipcServer.OnLog -= OnServerLog;
                ipcServer.OnError -= OnServerError;
                ipcServer = null;
                isServerRunning = false;
            }
        }

        [ContextMenu("Test Reflection")]
        public void TestReflection()
        {
            Debug.Log("Testing Assembly-CSharp reflection...");
            var data = AssemblyReflector.ReflectAssemblyCSharp();
            Debug.Log($"Assembly: {data.AssemblyName}");
            Debug.Log($"Total types found: {data.Types.Count}");

            if (data.Types.Count > 0)
            {
                var firstType = data.Types[0];
                Debug.Log($"First type: {firstType.FullName}");
                Debug.Log($"  Fields: {firstType.Fields.Count}");
                Debug.Log($"  Methods: {firstType.Methods.Count}");
                Debug.Log($"  Properties: {firstType.Properties.Count}");
            }
        }

        private void OnServerLog(string message)
        {
            Debug.Log($"[IPC] {message}");
        }

        private void OnServerError(string message)
        {
            Debug.LogError($"[IPC] {message}");
        }
    }
}
