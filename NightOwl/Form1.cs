using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace NightOwl
{
    public partial class NightOwlForm : Form
    {
        public NightOwlForm()
        {
            InitializeComponent();
            ShaderDirectoryWatcherPathRichTextBox.Text = Directory.GetCurrentDirectory();
        }
        private void toolStripMenuItemFileDropDownMenuItemExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
        private void BrowseButton_Click(object sender, EventArgs e)
        {
            if(string.IsNullOrWhiteSpace(value: ShaderDirectoryWatcherPathRichTextBox.Text))
            {
                FolderBrowserDialog.SelectedPath = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
            }
            else
            {
                FolderBrowserDialog.SelectedPath = ShaderDirectoryWatcherPathRichTextBox.Text;
            }

            DialogResult result = FolderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                var path = FolderBrowserDialog.SelectedPath;
                try
                {
                    if (!string.IsNullOrWhiteSpace(value: ShaderDirectoryWatcherPathRichTextBox.Text))
                    {
                        ShaderDirectoryWatcherRunButton.Enabled = true;
                        ShaderDirectoryWatcherPathRichTextBox.Text = path;
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"An error occurred.{ex}");
                }
                //Invalidate();
            }
            // Cancel button was pressed.
            else if (result == DialogResult.Cancel)
            {
                if(string.IsNullOrWhiteSpace(value: ShaderDirectoryWatcherPathRichTextBox.Text))
                {
                    ShaderDirectoryWatcherRunButton.Enabled = false;
                }
                return;
            }
        }
        private void ShaderDirectoryWatcherRunButton_Click(object sender, EventArgs e)
        {
            MethodInvoker logWacthingUpdatedDirectory = () => ShaderDirectoryWatcherLogRichTextBox.AppendText($"Watcthing: {ShaderDirectoryWatcherPathRichTextBox.Text}\u2028");
            Invoke(logWacthingUpdatedDirectory);
            FileSystemWatcher.Path = ShaderDirectoryWatcherPathRichTextBox.Text;
        }
        private void ClearButton_Click(object sender, EventArgs e)
        {
            MethodInvoker action = () => ShaderDirectoryWatcherLogRichTextBox.Text = "";
            Invoke(action);
        }
        private void OnFileCreated(object sender, FileSystemEventArgs e)
        {
            MethodInvoker logCreatedFile = () => {
                ShaderDirectoryWatcherLogRichTextBox.AppendText($"Created: {e.FullPath}\u2028");
                ShaderDirectoryWatcherLogRichTextBox.ScrollToCaret();
            };
            Invoke(logCreatedFile);
        }
        private void OnFileRenamed(object sender, RenamedEventArgs e)
        {
            MethodInvoker logRenamedFile = () => {
                ShaderDirectoryWatcherLogRichTextBox.AppendText($"Renamed: {e.FullPath}\u2028");
                ShaderDirectoryWatcherLogRichTextBox.AppendText($"    Old: {e.OldFullPath}\u2028");
                ShaderDirectoryWatcherLogRichTextBox.ScrollToCaret();
            };
            Invoke(logRenamedFile);
        }
        private void OnFileChanged(object sender, FileSystemEventArgs e)
        {
            FileAttributes attributes = File.GetAttributes(e.FullPath);
            if ((attributes & FileAttributes.Directory) == 0)
            {
                try
                {
                    if (!e.Name.EndsWith(".spv"))
                    {
                        MethodInvoker logModifiedFile = () => {
                            ShaderDirectoryWatcherLogRichTextBox.AppendText($"Modified: {e.FullPath}\u2028");
                            ShaderDirectoryWatcherLogRichTextBox.ScrollToCaret();
                        };
                        Invoke(logModifiedFile);
                        string VulkanDirectory = "";
                        MethodInvoker outData = () => VulkanDirectory = VulkanDirectoryRichTextBox.Text;
                        Invoke(outData);
                        //if (string.IsNullOrEmpty(VulkanDirectory)) return;
                        string watchPath = (sender as FileSystemWatcher)?.Path;
                        StringBuilder commandBulder = new("/C ");
                        commandBulder
                            .Append(VulkanDirectory)
                            .Append(' ')
                            .Append(watchPath)
                            .Append('/')
                            .Append(e.Name)
                            .Append(" -o ")
                            .Append(watchPath)
                            .Append('/')
                            .Append(e.Name)
                            .Append(".spv");
                        MethodInvoker logExecutingCommand = () => {
                            ShaderDirectoryWatcherLogRichTextBox.AppendText($"Execute: {commandBulder}\u2028");
                            ShaderDirectoryWatcherLogRichTextBox.ScrollToCaret();
                        };
                        Invoke(logExecutingCommand);
                        var StartInfo = new ProcessStartInfo
                        {
                            UseShellExecute = false,
                            FileName = "cmd.exe",
                            RedirectStandardOutput = true,
                            RedirectStandardError = true,
                            CreateNoWindow = true,
                            Arguments = commandBulder.ToString()
                        };
                        Process process = Process.Start(StartInfo);
                        process.ErrorDataReceived += (s, e) => {
                            MethodInvoker outputError = () => {
                                ShaderDirectoryWatcherLogRichTextBox.AppendText($"{e.Data}\u2028");
                                ShaderDirectoryWatcherLogRichTextBox.ScrollToCaret();
                            };
                            Invoke(outputError);
                        };
                        process.OutputDataReceived += (s, e) => {
                            if (e.Data?.Length > 0)
                            {
                                MethodInvoker outputReceivedData = () => {
                                    ShaderDirectoryWatcherLogRichTextBox.AppendText($"{e.Data}\u2028");
                                    ShaderDirectoryWatcherLogRichTextBox.ScrollToCaret();
                                };
                                Invoke(outputReceivedData);
                            }
                            else
                            {
                                MethodInvoker outputComplete = () => {
                                    ShaderDirectoryWatcherLogRichTextBox.AppendText("Completed.\u2028");
                                    ShaderDirectoryWatcherLogRichTextBox.ScrollToCaret();
                                };
                                Invoke(outputComplete);
                            }
                        };
                        process.Start();
                        process.BeginErrorReadLine();
                        process.BeginOutputReadLine();

                        process.WaitForExit();
                    }
                    else
                    {
                        //var fileInfo = new FileInfo(e.FullPath);
                        //if(fileInfo.Length > 0)
                        //{
                        //    MethodInvoker logModifiedFile = () => {
                        //        ShaderDirectoryWatcherRichTextBox.AppendText($"Modified: {e.FullPath}\u2028");
                        //        ShaderDirectoryWatcherRichTextBox.ScrollToCaret();
                        //    };
                        //    Invoke(logModifiedFile);
                        //}
                    }
                }
                catch (Exception ex)
                {
                    PrintException(ex);
                }
            }
        }
        private void OnFileError(object sender, ErrorEventArgs e) => PrintException(e.GetException());
        private void PrintException(Exception ex)
        {
            if (ex != null)
            {
                MethodInvoker logError = () => {
                    ShaderDirectoryWatcherLogRichTextBox.AppendText($"Message: {ex.Message}");
                    ShaderDirectoryWatcherLogRichTextBox.AppendText("Stacktrace:");
                    ShaderDirectoryWatcherLogRichTextBox.AppendText("Error:");
                    ShaderDirectoryWatcherLogRichTextBox.AppendText(ex.StackTrace);
                    ShaderDirectoryWatcherLogRichTextBox.AppendText("\u2028");
                    ShaderDirectoryWatcherLogRichTextBox.ScrollToCaret();
                };
                Invoke(logError);
                PrintException(ex.InnerException);
            }
        }
        private void OpenButton_Click(object sender, EventArgs e)
        {
            try
            {
                Process.Start(new ProcessStartInfo()
                {
                    FileName = ShaderDirectoryWatcherPathRichTextBox.Text,
                    UseShellExecute = true,
                    Verb = "open"
                });
            }
            catch(Exception ex)
            {
                PrintException(ex.InnerException);
            }
        }
    }
}
