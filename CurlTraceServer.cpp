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
#include <limits.h>
#include <stdlib.h>
#include "xmlWriter/ExportManifestXml.h"

struct MemoryStruct
{
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr)
  {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

void ServerHealth()
{
  CURL *curl_handle;
  CURLcode res;
  struct MemoryStruct chunk;
  chunk.memory = (char *)malloc(1); /* will be increased by realloc */
  chunk.size = 0;
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(curl_handle, CURLOPT_URL, "http://0.0.0.0:8080/tsp/api/health");
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_DEFAULT_PROTOCOL, "https");
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
  Json::Value jsonData;
  Json::Reader jsonReader;
  long httpCode(0);
  std::unique_ptr<std::string> httpData(new std::string());
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  res = curl_easy_perform(curl_handle);
  if (res != CURLE_OK)
  {
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
  }
  else
  {
    std::string serverHealthStatus = "DOWN";
    if (jsonReader.parse(chunk.memory, jsonData))
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

void verifyPath(std::string path)
{
  DIR *dir = opendir(path.c_str());
  if (dir)
  {
    /* Directory exists. */
  }
  else if (ENOENT == errno)
  {
    /* Directory does not exist. */
    std::cout << "Error! The directory: \n"
              << path << "\ndoes not exist"
              << "\n";
    exit(EXIT_FAILURE);
  }
  else
  {
    std::cout << "Error! Failed to open the directory: \n"
              << path << "\n";
    exit(EXIT_FAILURE);
  }
}

std::string curlRequest(const char *data, const char *mydata, const char *parsingKey)
{
  CURL *curl_handle;
  CURLcode res;
  std::string requestStatus = "No data available";
  struct MemoryStruct chunk;

  chunk.memory = (char *)malloc(1); /* will be increased by realloc */
  chunk.size = 0;
  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* specifying URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(curl_handle, CURLOPT_URL, mydata);
  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_DEFAULT_PROTOCOL, "https");

  /* adding json headers */
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
  Json::Value jsonData;
  Json::Reader jsonReader;
  long httpCode(0);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* passing 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  /* providing user-agent field */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  res = curl_easy_perform(curl_handle);

  /* checking for errors */
  if (res != CURLE_OK)
  {
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
  }
  else
  {
    if (jsonReader.parse(chunk.memory, jsonData))
    {
      const std::string request_Status(jsonData[parsingKey].asString());
      requestStatus = request_Status;
    }
    else
      std::cout << "An error happened during parsing: \n" <<  jsonReader.getFormattedErrorMessages() << std::endl;
  }

  /* cleanup */
  curl_easy_cleanup(curl_handle);
  free(chunk.memory);
  curl_global_cleanup();

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
  if (argc < 3)
  {
    std::cout << "Error: there are missing arguments, the program can not run properly \n";
    return EXIT_FAILURE;
  }
  char resolved_path[PATH_MAX];
  verifyPath(argv[1]);
  realpath(argv[1], resolved_path);
  std::string traces_path = resolved_path;
  verifyPath(argv[2]);
  realpath(argv[2], resolved_path);
  std::string trace_server_workspace = resolved_path;
  std::string str_analyses_indexes = argv[3];
  std::string create_tracePackage;

  if (argv[4] != NULL)
  {
    create_tracePackage = argv[4];
    if (create_tracePackage != "-p" && create_tracePackage != "--package")
      std::cout << "Error: No argument matches \"" + create_tracePackage + "\". Only \"-p\" or \"--package\" are supported. \n";
  }
  else
    create_tracePackage = "null";

  std::vector<int> int_analyses_indexes;
  char char_array[str_analyses_indexes.size()];
  strcpy(char_array, str_analyses_indexes.c_str());
  for (int i = 1; i < str_analyses_indexes.size() - 1; i += 2)
  {
    int_analyses_indexes.push_back(std::atoi(&char_array[i]));
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

  //Creating the experiment
  s = "{\"parameters\":{\"name\": \"theExperiment\",\"traces\": [" + uuids + "]}}";
  const std::string expUuid = curlRequest(s.c_str(), "http://0.0.0.0:8080/tsp/api/experiments", "UUID");
  std::cout << "\nINDEXING STARTED: \n";
  std::cout << "INDEXING STATUS = RUNNING ... \n";


  // Waiting for indexing to complete
  std::string indexingStatus = "";
  while (indexingStatus.compare("COMPLETED") != 0)
  {
    indexingStatus = curlRequest(s.c_str(), "http://0.0.0.0:8080/tsp/api/experiments", "indexingStatus");
    usleep(1000000); /* adding 1s sleep to reduce the polling request frequency  */
  }
  std::cout << "INDEXING STATUS = " + indexingStatus + "\n";

  //Lanching the desired analysis via their data providers and waiting for them to complete
  for (int i = 0; i < int_analyses_indexes.size(); i++)
  {
    if (data_providers.size() > 0 && int_analyses_indexes[i] < data_providers.size())
    {
      std::cout << "\nANALYSIS COMPUTING STARTED: " + analysis_names[int_analyses_indexes[i]] + "\n";
      std::cout << "ANALYSIS STATUS = RUNNING ... \n";

      const std::string mystr = "http://localhost:8080/tsp/api/experiments/" + expUuid + data_providers[int_analyses_indexes[i]];
      s = "{\"parameters\":{\"requested_times\":" + requested_times + "," + "\"requested_items\":" + requested_items + "}}";
      data = s.c_str();
      const char *mydata = mystr.c_str();
      std::string status = "";
      while (status.compare("COMPLETED") != 0)
      {
        status = curlRequest(data, mydata, "status");
        usleep(1000000); /* adding 1s sleep to reduce the polling request frequency  */
      }
      std::cout << "ANALYSIS STATUS = " + status + "\n";
    }
    else
      std::cout << "Invalid required analyses values" << std::endl;
  }

  if (create_tracePackage == "-p" || create_tracePackage == "--package")
  {    
    // Creating the trace package (zip file)
    std::cout << "\nCREATING THE TRACE PACKAGE ...\n";
    std::string shell = "./copyingData.sh";
    std::string command = shell + " " + traces_path + " " + trace_server_workspace;
    std::system(command.c_str());

    CreateExportManifestXml(trace_server_workspace);
    shell = "./createZip.sh";
    command = shell + " " + trace_server_workspace;
    system(command.c_str());
    std::cout << "TRACE PACKAGE CREATED AT: " + trace_server_workspace + "/archiveData" + "\n";

  }
  return 0;
}
