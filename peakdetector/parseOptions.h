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
        ParseOptions(struct Arguments &arguments);
        ~ParseOptions();
        void createXMLFile(struct Arguments &arguments);
        void addChildren(xml_node args, char* nodeName, QStringList cliArguments);
        char* qStringtocharPointer(QString stringToBeConverted);

        template <typename T> 
        xml_node addNode(T &doc, char* nodeName, char* nodeValue = 0);

        template <typename T>
        void addAttribute(xml_node &node, char* attrName, T value);

        void saveDoc(xml_document &doc, char* docPath);

        void checkErrors(xml_parse_result &result, char* source);
};

#endif //PARSEOPTIONS_H