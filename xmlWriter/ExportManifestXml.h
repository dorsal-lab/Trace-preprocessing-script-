#ifndef ExportManifestXml_H
#define ExportManifestXml_H

#include "XmlWriter.h"
#include <dirent.h>
#include <algorithm>

std::vector<std::string> getDirectories(const char *PATH);
std::vector<std::string> getFiles(const char *PATH);
void CreateExportManifestXml(std::string TRACE_SERVER_WORKSPACE);

#endif