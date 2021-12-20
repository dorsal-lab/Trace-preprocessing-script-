#include <iostream>
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
  //Posting the traces on the trace-server




  std::vector<std::string> traces = getDirectories(traces_path.c_str());
  std::string uuids = "\"";
  const char *data = "";
  std::string s = "";



  for (int i = 0; i < traces.size(); i++)
  {
    std::string s = "{\"parameters\":{\"uri\": \"" + traces_path + "/" + traces[i] + "/kernel\",\"name\": \"" + "kernel" + "\"}}";
    data = s.c_str();

    const std::string uuid = curlRequest(data, "http://0.0.0.0:8080/tsp/api/traces", "UUID");
    if (i < traces.size() - 1)
      uuids = uuids + uuid + "\",\"";
    else
      uuids = uuids + uuid + "\"";
  }

  std::cout << "uuids= " + uuids << std::endl;

  //Creating the experiment
  s = "{\"parameters\":{\"name\": \"LoveExp\",\"traces\": [" + uuids + "]}}";
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
      const std::string mystr = "http://localhost:8080/tsp/api/experiments/" + expUuid + data_providers[i];
      s = "{\"parameters\":{\"requested_times\":" + requested_times + "," + "\"requested_items\":" + requested_items + "}}";
      data = s.c_str();
      const char *mydata = mystr.c_str();
      std::string status = "";
      while (status.compare("COMPLETED") != 0)
      {
        std::cout << "Request Updated Status = " << status << "\n";
        status = curlRequest(data, mydata, "status");
        usleep(1000000); // 1s
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


// faire en sorte que les arguments qui donneront les analyses que le user veut lancer acceptent aussi des indices a deux chiffres en utilisant
// des tableau de chaines de caractes ou bien listes au lieu de tableau de caractere

//  completer la liste des data providers des autres analyses dans le config.h

// regler ou regarder quoi faire pour les mesages d'erreur:
// Error: File alread exists.
//Error opening file.
// qui s'affichent quand je relance l'analyse, which means les fichiers de archive data existent deja

// voir si je garde l'archive file dans le server workspace ou je le change.

// changer l'ordre des arguments que doit rentrer l'utilisateur: 1/traces_path  2/ analyses_indexes 3/create_archive ? 4/ path_to_trace_server_home  


// Ajouter des descriptifs des fonctions comme dans le xmlWriter

// mettre le tt dans le docker 

// rendre le chemin vers le trace server relatif quand j'aurais tt mis dans le docker

// tester ensuite sur une autre machine 

// OS execution graph ou bien critical path analysis doit pouvoir etre lancee en batch elle aussi, penser a comment faire ca vu que elle 
//prend un thread bien precis peut etre ?!

// regarder si je garde les requested_times et requested_itmes tel quels ou pas
