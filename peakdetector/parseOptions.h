#ifndef PARSEOPTIONS_H
#define PARSEOPTIONS_H

#include <iostream>
#include <string>

#include "pugixml.hpp"

#include "PeakDetectorCLI.h"

using namespace std;
using namespace pugi;

class ParseOptions {

    public:
        ParseOptions(const struct Arguments &args);
        ~ParseOptions();
        void createXMLFile();
        template <typename T> 
        xml_node addNode(T &doc, char* nodeName, char* nodeValue = 0);

        template <typename T>
        void addAttribute(xml_node &node, char* attrName, T value);

        void saveDoc(xml_document &doc, char* docPath);

        void checkErrors(xml_parse_result &result, char* source);
};

#endif //PARSEOPTIONS_H