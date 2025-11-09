using MelonLoader;
using System;
using System.Reflection;

[assembly: MelonInfo(typeof(UnityReflectionMod.ReflectionMod), "Unity Reflection Mod", "1.0.0", "Your Name")]
[assembly: MelonGame(null, null)] // Works with any Unity game

namespace UnityReflectionMod
{
    public class ReflectionMod : MelonMod
    {
        private IPCServer? ipcServer;

        public override void OnInitializeMelon()
        {
            LoggerInstance.Msg("Unity Reflection Mod initialized!");
            LoggerInstance.Msg("Starting IPC server...");

            try
            {
                // Create and start IPC server
                ipcServer = new IPCServer();
                ipcServer.OnLog += (msg) => LoggerInstance.Msg($"[IPC] {msg}");
                ipcServer.OnError += (msg) => LoggerInstance.Error($"[IPC] {msg}");
                ipcServer.Start();

                LoggerInstance.Msg("IPC server started. Waiting for viewer to connect...");
                LoggerInstance.Msg("The viewer will receive Assembly-CSharp reflection data on connection.");
            }
            catch (Exception ex)
            {
                LoggerInstance.Error($"Failed to start IPC server: {ex.Message}");
                LoggerInstance.Error(ex.StackTrace);
            }
        }

        public override void OnDeinitializeMelon()
        {
            LoggerInstance.Msg("Shutting down IPC server...");
            ipcServer?.Stop();
            LoggerInstance.Msg("Unity Reflection Mod unloaded.");
        }

        public override void OnSceneWasLoaded(int buildIndex, string sceneName)
        {
            LoggerInstance.Msg($"Scene loaded: {sceneName} (index: {buildIndex})");
        }
    }
}
