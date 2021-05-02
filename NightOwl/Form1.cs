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
            FileSystemWatcher = new(ShaderDirectoryWatcherPathRichTextBox.Text);
            (FileSystemWatcher as System.ComponentModel.ISupportInitialize)?.BeginInit();
            FileSystemWatcher.NotifyFilter = NotifyFilters.FileName | NotifyFilters.LastWrite;
            FileSystemWatcher.EnableRaisingEvents = true;
            FileSystemWatcher.IncludeSubdirectories = true;
            //FileSystemWatcher.SynchronizingObject = this;
            FileSystemWatcher.Created += OnCreated;
            FileSystemWatcher.Renamed += OnRenamed;
            FileSystemWatcher.Changed += OnChanged;
            FileSystemWatcher.Error += OnError;
            (FileSystemWatcher as System.ComponentModel.ISupportInitialize)?.EndInit();
        }

        private void OnCreated(object sender, FileSystemEventArgs e)
        {
            MethodInvoker action = () => {
                ShaderDirectoryWatcherRichTextBox.AppendText($"Created: {e.FullPath}\u2028");
                ShaderDirectoryWatcherRichTextBox.ScrollToCaret();
            };
            Invoke(action);
        }

        private void OnRenamed(object sender, RenamedEventArgs e)
        {
            MethodInvoker action = () => {
                ShaderDirectoryWatcherRichTextBox.AppendText($"Renamed: {e.FullPath}\u2028");
                ShaderDirectoryWatcherRichTextBox.AppendText($"    Old: {e.OldFullPath}\u2028");
                ShaderDirectoryWatcherRichTextBox.AppendText($"    New: {e.FullPath}\u2028");
            };
            Invoke(action);
        }

        private void OnChanged(object sender, FileSystemEventArgs e)
        {
            FileAttributes attributes = File.GetAttributes(e.FullPath);
            if((attributes & FileAttributes.Directory) == 0)
            {
                try
                {
                    string VulkanDirectory = "";
                    MethodInvoker outData = () => VulkanDirectory = VulkanDirectoryRichTextBox.Text;
                    Invoke(outData);
                    //if (string.IsNullOrEmpty(VulkanDirectory)) return;
                    string watchPath = (sender as FileSystemWatcher)?.Path;
                    StringBuilder commandBulder = new("/C ");
                    commandBulder.Append(VulkanDirectory).Append(' ').Append(watchPath).Append('/').Append(e.Name).Append(" -o ").Append(watchPath).Append('/').Append(e.Name).Append(".spv");
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
                            ShaderDirectoryWatcherRichTextBox.AppendText($"{e.Data}\u2028");
                            ShaderDirectoryWatcherRichTextBox.ScrollToCaret();
                        };
                        Invoke(outputError);
                    };
                    process.OutputDataReceived += (s, e) => {
                        MethodInvoker outData = () => {
                            ShaderDirectoryWatcherRichTextBox.AppendText($"{e.Data}\u2028");
                            ShaderDirectoryWatcherRichTextBox.ScrollToCaret();
                        };
                        Invoke(outData);
                    };
                    process.Start();
                    process.BeginErrorReadLine();
                    process.BeginOutputReadLine();

                    process.WaitForExit();
                }
                catch (Exception ex)
                {
                    PrintException(ex);
                }
            }
        }

        private void OnError(object sender, ErrorEventArgs e) => PrintException(e.GetException());

        private void PrintException(Exception ex)
        {
            if (ex != null)
            {
                MethodInvoker action = () => {
                    ShaderDirectoryWatcherRichTextBox.AppendText($"Message: {ex.Message}");
                    ShaderDirectoryWatcherRichTextBox.AppendText("Stacktrace:");
                    ShaderDirectoryWatcherRichTextBox.AppendText("Error:");
                    ShaderDirectoryWatcherRichTextBox.AppendText(ex.StackTrace);
                    ShaderDirectoryWatcherRichTextBox.AppendText("\u2028");
                    ShaderDirectoryWatcherRichTextBox.ScrollToCaret();
                };
                Invoke(action);
                PrintException(ex.InnerException);
            }
        }
    }
}
