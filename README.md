# Trace preprocessing script

This script allows the preprocessing of a trace or a group of traces in order to be analyzed afterwards. It can be especially useful with large traces, in that case, the user can just launch the script by choosing which analyzes he wishes and let it run until it completes. The main advantage of this script, is that once the preprocessing is over, the user can open the trace-server workspace with Trace Compass or Trace Viewer extension, or import a trace package on Trace Compass, and then, open all the views of the preprocessed analyzes instantly.



## Dependencies:

In order to use this script, you should make sure of having the following dependencies:

**- The trace-server:** 
Before running the script, the trace-server should be started at its default port 8080. To do so, if you already have a trace-server package on your machine, you can go to its main folder and open a terminal from there, and then run the following command:

```
./tracecompass-server
```

If you do not have a trace-server package, You can get a one by either building your own version, or just downloading it from [this link](https://download.eclipse.org/tracecompass.incubator/trace-server/rcp/).

To build a trace-server, you may do what follows:
1. Clone the trace Compass incubator repository using:

```
git clone https://git.eclipse.org/r/tracecompass.incubator/org.eclipse.tracecompass.incubator
```

2. From the incubator folder, open a terminal and start the `mvn` build with:

```
mvn clean install -DskipTests=true
```

3. Once the build completes, you will find your trace-server ready for use under the folder:

```
org.eclipse.tracecompass.incubator/trace-server/org.eclipse.tracecompass.incubator.trace.server.product/target/products/traceserver/linux/gtk/x86_64/trace-compass-server
```


**- libcurl4-openssl-dev package :** libcurl is an easy-to-use client-side URL transfer library, this package provides the development files  that will allow you to build the script. It can be installed by executing the following commands:

```  
sudo apt-get update 
sudo apt-get install libcurl4-openssl-dev
```


**- libjsoncpp-dev package:**

*libjsoncpp-dev* can be installed by using the following commands:
```
sudo apt-get install libjsoncpp-dev
sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json
```

**- g++:**
It can be installed using the command below:

```  
sudo apt install g++ 
```

## Usage
First, you need to build the project using the following command:

```
g++ CurlTraceServer.cpp xmlWriter/XmlWriter.cpp xmlWriter/ExportManifestXml.cpp -ljsoncpp -lcurl -o pp_script.out
```

then, you can launch the script with the necessary arguments as follows:
```
./pp_script.out  /Path_to_the_traces_directory/  /Path_to_the_trace-server_workspace 0 [0,1,2]
```

As you can see, you should enter 4 arguments:

1. The absolute path to the traces folder 
2. The absolute path to the trace-server workspace that may look like: `/home/ubuntu/.tracecompass-webapp`
3. A parameter that you should set to 1 if you want to create a trace Package, you can set it to 0 otherwise.
4. An array giving the indexes of the different analyses you want to preprocess. The numbers should be separated with comma's. For example to run the analyses corresponding to the indexes 0, 1 and 3 in the "config.h" file of the project, you may use this array: [0,1,3]. If you want to do the indexing with no additional analyses, you can use an empty array. In order to find the index of a specific analysis (data provider), you can have a look at the "config.h" file. Alternatively, you can use the table below. More analysis will be added.

| Analyis             | Index  |
| ------------------- | -----  |
| Kernel Ressources   | 0      |
| Cpu usage           | 1      |
| Memory usage        | 2      |
| System call         | 3      |


## Visualization of the results
Once the server finishes the preprocessing, you can open the analyses views by one of the following options:

#### Option 1: using the `Trace Viewer extension`
Please have a look at [this page](https://github.com/theia-ide/theia-trace-extension) for more details.

#### Option 2: Opening the trace-server workspace with the Trace Compass RCP
1. Make sure to stop the trace-server to make its workspace available for reading by another application (Trace Compass)
2. You can go to the root folder of Trace Compass and open a terminal from there. If you do not have Trace Compass on your machine, you can download it from [here](https://www.eclipse.org/tracecompass/). 
3. Now, you can open the trace-server workspace using the Trace Compass RCP with the following command:
```
./tracecompass -data /path_to_the_trace-server_workspace
```
The path to the trace-server may look like:
```
$ ./tracecompass -data ~/.tracecompass-webapp
```
You should be able to see your traces from the **Project Explorer** of Trace Compass now. If not, try to make a `Refresh` by right clicking and choosing `Refresh`.


#### Option 3: Importing a trace Package into Trace Compass
If you put the third argument at 1, you have already created a Trace Package containing all the analyses files and that you may import to Trace Compass by following the steps explained in [this page](https://archive.eclipse.org/tracecompass/doc/stable/org.eclipse.tracecompass.doc.user/Trace-Compass-Main-Features.html#Importing_2).


