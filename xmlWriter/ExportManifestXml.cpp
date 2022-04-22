#include "ExportManifestXml.h"

std::vector<std::string> getDirectories(const char *PATH)
{
    std::string myPATH = PATH;
    DIR *dir = opendir(PATH);
    if (dir)
    {
        /* Directory exists. */
    }
    else if (ENOENT == errno)
    {
        /* Directory does not exist. */
        std::cout << "The directory: \n" + myPATH + "\n does not exist"
                  << "\n";
    }
    else
    {
        std::cout << "Failed to open the directory: \n" + myPATH << "\n";
    }
    std::vector<std::string> directories;
    struct dirent *entry = readdir(dir);
    while (entry != NULL)
    {
        std::string s = entry->d_name;
        if (entry->d_type == DT_DIR && s != "." && s != "..")
            directories.push_back(entry->d_name);
        entry = readdir(dir);
    }
    closedir(dir);
    return directories;
}

std::vector<std::string> getFiles(const char *PATH)
{
    DIR *dir = opendir(PATH);
    std::vector<std::string> files;
    struct dirent *entry = readdir(dir);
    while (entry != NULL)
    {
        std::string s = entry->d_name;
        if (entry->d_type != DT_DIR)
            files.push_back(entry->d_name);
        entry = readdir(dir);
    }
    closedir(dir);
    return files;
}

void CreateExportManifestXml(std::string TRACE_SERVER_WORKSPACE)
{
    std::string SUPP_FILES_PATH = TRACE_SERVER_WORKSPACE + "/archiveData/.tracing";
    std::vector<std::string> main_directories = getDirectories(SUPP_FILES_PATH.c_str());
    XmlWriter xml;
    if (xml.open(TRACE_SERVER_WORKSPACE + "/archiveData/export-manifest.xml"))
    {
        xml.writeOpenTag("tmf-export");
        for (int i = 0; i < main_directories.size(); i++)
        {
            std::string directory_path = SUPP_FILES_PATH + "/" + main_directories[i];
            std::vector<std::string> sub_directories = getDirectories(directory_path.c_str());

            // if sub_directories.size() > 0  we enter the for loop (we have )

            if (sub_directories.size() < 1) // we have only one trace
            {
                std::vector<std::string> sub_directory_files = getFiles(directory_path.c_str());

                xml.writeStartOpenTag("trace");
                xml.writeAttribute("name=\"" + main_directories[i] + "\"");
                xml.writeAttribute("type=\"org.eclipse.linuxtools.lttng2.kernel.tracetype\"");
                xml.writeEndOpenTag();

                xml.writeStartElementTag("file");
                xml.writeAttribute("name=\"" + main_directories[i] + "\"");
                xml.writeEndElementTag();

                for (int k = 0; k < sub_directory_files.size(); k++)
                {
                    xml.writeStartElementTag("supplementary-file");
                    xml.writeAttribute("name=\".tracing/" + main_directories[i] + "/" + sub_directory_files[k] + "\"");
                    xml.writeEndElementTag();
                }
                xml.writeCloseTag();
            }
            else
            { 
                for (int j = 0; j < sub_directories.size(); j++)
                {
                    std::string sub_directory_path = directory_path + "/" + sub_directories[j];
                    std::vector<std::string> sub_directory_files = getFiles(sub_directory_path.c_str());

                    xml.writeStartOpenTag("trace");
                    xml.writeAttribute("name=\"" + sub_directories[j] + "\"");
                    xml.writeAttribute("type=\"org.eclipse.linuxtools.lttng2.kernel.tracetype\"");
                    xml.writeEndOpenTag();

                    xml.writeStartElementTag("file");
                    xml.writeAttribute("name=\"" + main_directories[i] + "/" + sub_directories[j] + "\"");
                    xml.writeEndElementTag();

                    for (int k = 0; k < sub_directory_files.size(); k++)
                    {
                        xml.writeStartElementTag("supplementary-file");
                        xml.writeAttribute("name=\".tracing/" + main_directories[i] + "/" + sub_directories[j] + "/" + sub_directory_files[k] + "\"");
                        xml.writeEndElementTag();
                    }
                    xml.writeCloseTag();
                }
            }
        }
        xml.writeCloseTag(); // tmf-export
        xml.close();
    }
    else
    {
        std::cout << "Error opening file.\n";
    }
}
