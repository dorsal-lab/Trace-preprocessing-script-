#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <json/json.h>
#include <memory>
#include <dirent.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include "config.h"
#include <list>
#include <unistd.h>
#include "xmlWriter/ExportManifestXml.h"

namespace
{
  std::size_t callback(
      const char *in,
      std::size_t size,
      std::size_t num,
      std::string *out)
  {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
  }
}

void ServerHealth()
{
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, "http://0.0.0.0:8080/tsp/api/health");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    Json::Value jsonData;
    Json::Reader jsonReader;
    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    res = curl_easy_perform(curl);
    std::string serverHealthStatus = "DOWN";
    if (jsonReader.parse(*httpData.get(), jsonData))
    {
      const std::string serverHealth(jsonData["status"].asString());
      serverHealthStatus = serverHealth;
    }
    if (serverHealthStatus == "UP")
    { /* Server is OK */
    }
    else
    {
      std::cout << "Error: Failed to connect to the trace-server" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

std::string curlRequest(const char *data, const char *mydata, const char *parsingKey)
{
  CURL *curl;
  CURLcode res;
  std::string requestStatus = "No data available";
  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_URL, mydata);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    Json::Value jsonData;
    Json::Reader jsonReader;
    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    res = curl_easy_perform(curl);
    if (jsonReader.parse(*httpData.get(), jsonData))
    {
      std::cout << jsonData.toStyledString() << std::endl;
      const std::string request_Status(jsonData[parsingKey].asString());
      requestStatus = request_Status;
    }
  }
  return requestStatus;
}

std::vector<std::string> execute(const char *cmd)
{
  std::array<char, 255> buffer;
  std::vector<std::string> result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe)
  {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
  {
    std::string str = buffer.data();
    //Removing the endline from the string
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    result.push_back(str);
  }
  return result;
}


int main(int argc, char **argv)
{
  std::string traces_path = argv[1];
  std::string trace_server_workspace = argv[2];
  std::string create_tracePackage = argv[3];
  std::string str_analyses_indexes = argv[4];
  
  std::vector<int> int_analyses_indexes;
  char char_array[str_analyses_indexes.size()];
  strcpy(char_array, str_analyses_indexes.c_str());
  for (int i=1; i < str_analyses_indexes.size()-1 ; i+=2 )
  {
    int_analyses_indexes.push_back((int)char_array[i] - 48); /* in ASCII code, the digits start from 48 */
    }

  if(argc < 3)
  {
    std::cout << "Error: there are missing arguments, the program can not run properly \n";
    return EXIT_FAILURE;
  }

  // Checking if the server is alive
  ServerHealth();

  //getting the list of the traces from the main folder given by the user
  std::string cmd = "find " + traces_path + " -type f -name 'metadata' | sed -r 's|/[^/]+$||' |sort |uniq";
  std::vector<std::string> traces = execute(cmd.c_str());
  
  std::string uuids = "\"";
  const char *data = "";
  std::string s = "";


  //Posting the traces on the trace-server
  for (int i = 0; i < traces.size(); i++)
  {
    std::string s = "{\"parameters\":{\"uri\": \"" + traces[i] + "\",\"name\": \"" + "kernel" + "\"}}";
    data = s.c_str();

    const std::string uuid = curlRequest(data, "http://0.0.0.0:8080/tsp/api/traces", "UUID");
    if (i < traces.size() - 1)
      uuids = uuids + uuid + "\",\"";
    else
      uuids = uuids + uuid + "\"";
  }

  std::cout << "uuids= " + uuids << std::endl;

  //Creating the experiment
  s = "{\"parameters\":{\"name\": \"theExperiment\",\"traces\": [" + uuids + "]}}";
  const std::string expUuid = curlRequest(s.c_str(), "http://0.0.0.0:8080/tsp/api/experiments", "UUID");
  std::cout << "Experiment UUID: " << expUuid << std::endl;

  // Waiting for indexing to complete
  std::string indexingStatus = "";
  while (indexingStatus.compare("COMPLETED") != 0)
  {
    std::cout << "Indexing Updated Status = " << indexingStatus << "\n";
    indexingStatus = curlRequest(s.c_str(), "http://0.0.0.0:8080/tsp/api/experiments", "indexingStatus");
    usleep(1000000); // 1s
  }
  std::cout << "Indexing Updated Status = " << indexingStatus << "\n";

  //Lanching the desired analysis via their data providers and waiting for them to complete
  for (int i = 0; i < int_analyses_indexes.size(); i++)
  {
    if (data_providers.size() > 0 && int_analyses_indexes[i] < data_providers.size())
    {
      const std::string mystr = "http://localhost:8080/tsp/api/experiments/" + expUuid + data_providers[int_analyses_indexes[i]];
      s = "{\"parameters\":{\"requested_times\":" + requested_times + "," + "\"requested_items\":" + requested_items + "}}";
      data = s.c_str();
      const char *mydata = mystr.c_str();
      std::string status = "";
      while (status.compare("COMPLETED") != 0)
      {
        std::cout << "Request Updated Status = " << status << "\n";
        status = curlRequest(data, mydata, "status");
        usleep(30000000); // 30s, this sleep time is needed to avoid parsing errors
      }
      std::cout << "Request Updated Status = " << status << "\n";
    }
    else
      std::cout << "Invalid required analyses values" << std::endl;
  }

if( create_tracePackage == "1")
{
    DIR *dir = opendir(trace_server_workspace.c_str());
    if (dir)
    {
        /* Directory exists. */
    }
    else if (ENOENT == errno)
    {
        /* Directory does not exist. */
        std::cout << "The directory: \n" << trace_server_workspace << "\n does not exist" << "\n";
        exit(EXIT_FAILURE);

    }
    else
    {
        std::cout << "Failed to open the directory: \n" << trace_server_workspace << "\n";
        exit(EXIT_FAILURE);
    }

  std::string shell = "./copyingData.sh";
  std::string command = shell + " " + traces_path + " " + trace_server_workspace;
  std::system(command.c_str());

  CreateExportManifestXml(trace_server_workspace);
  // Creating the trace package (zip file)
  shell = "./createZip.sh";
  command = shell + " " + trace_server_workspace;
  system(command.c_str());
}
  return 0;
}
