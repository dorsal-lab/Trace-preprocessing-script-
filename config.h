#ifndef config_H
#define config_H

#include <string>
#include <vector>
std::vector<std::string> data_providers = {"/outputs/timeGraph/org.eclipse.tracecompass.internal.analysis.os.linux.core.threadstatus.ResourcesStatusDataProvider/tree",
                                           "/outputs/XY/org.eclipse.tracecompass.analysis.os.linux.core.cpuusage.CpuUsageDataProvider/tree",
                                           "/outputs/XY/org.eclipse.tracecompass.analysis.os.linux.core.kernelmemoryusage/tree",
                                           "/outputs/XY/org.eclipse.tracecompass.internal.analysis.timing.core.segmentstore.scatter.dataprovider:org.eclipse.tracecompass.analysis.os.linux.latency.syscall/tree"};

std::vector<std::string> analysis_names = {"Kernel Analysis",
                                           "Cpu Usage Analysis",
                                           "Kernel Memory Usage Analysis",
                                           "Latency Syscall Analysis"};
std::string requested_times = "[0,100000000000]";
std::string requested_items = "[]";

#endif
