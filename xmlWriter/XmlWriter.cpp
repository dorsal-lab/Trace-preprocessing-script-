#include "XmlWriter.h"

bool XmlWriter::exists(const std::string fileName){
    std::fstream checkFile(fileName);
    return checkFile.is_open();
}

bool XmlWriter::open(const std::string strFile) {

    if (exists(strFile)){
        std::cout << "Error: File alread exists.\n";
        return false;
    }

    outFile.open(strFile);
    if (outFile.is_open()) {
        std::cout << "Xml file created.\n";
        indent = 0;
        openTags = 0;
        openElements = 0;
        return true;
    }

    return false;
}

void XmlWriter::close() {
    if (outFile.is_open()) {
        outFile.close();
    }
    else {
        std::cout << "File already closed.\n";
    }

}

void XmlWriter::writeOpenTag(const std::string openTag) {
    if (outFile.is_open()) {
        for (int i = 0; i < indent; i++) {
            outFile << "\t";
        }
        tempOpenTag.resize(openTags + 1);
        outFile << "<" << openTag << ">\n";
        tempOpenTag[openTags] = openTag;
        indent += 1;
        openTags += 1;
    }
    else {
        std::cout << "File is closed. Unable to write to file.\n";
    }
}


void XmlWriter::writeStartOpenTag(const std::string openTag) {
    if (outFile.is_open()) {
        for (int i = 0; i < indent; i++) {
            outFile << "\t";
        }
        tempOpenTag.resize(openTags + 1);
        outFile << "<" << openTag;
        tempOpenTag[openTags] = openTag;
        indent += 1;
        openTags += 1;
    }
    else {
        std::cout << "File is closed. Unable to write to file.\n";
    }
}

void XmlWriter::writeEndOpenTag() {
        if (outFile.is_open()) {
        outFile << " >\n";
    }
    else {
        std::cout << "File is closed. Unable to write to file.\n";
    }
}

void XmlWriter::writeCloseTag() {
    if (outFile.is_open()) {
        indent -= 1;
        for (int i = 0; i < indent; i++) {
            outFile << "\t";
        }
        outFile << "</" << tempOpenTag[openTags - 1] << ">\n";
        tempOpenTag.resize(openTags - 1);
        openTags -= 1;
    }
    else {
        std::cout << "File is closed. Unable to write to file.\n";
    }
}

void XmlWriter::writeStartElementTag(const std::string elementTag) {
    if (outFile.is_open()) {
        for (int i = 0; i < indent; i++) {
            outFile << "\t";
        }
        tempElementTag.resize(openElements + 1);
        tempElementTag[openElements] = elementTag;
        openElements += 1;
        outFile << "<" << elementTag;
    }
    else {
        std::cout << "File is closed. Unable to write to file.\n";
    }
}


void XmlWriter::writeEndElementTag() {
    if (outFile.is_open()) {
        outFile << " />\n";
        tempElementTag.resize(openElements - 1);
        openElements -= 1;
    }
    else {
        std::cout << "File is closed. Unable to write to file.\n";
    }
}

void XmlWriter::writeAttribute(const std::string outAttribute) {
    if (outFile.is_open()) {
        outFile << " " << outAttribute;
    }
    else {
        std::cout << "File is closed. Unable to write to file.\n";
    }
}

void XmlWriter::writeString(const std::string outString) {
    if (outFile.is_open()) {
        outFile << ">" << outString;
    }
    else {
        std::cout << "File is closed. Unable to write to file.\n";
    }
}
