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
        ParseOptions();
        ~ParseOptions();

		/**
		* [get map (name, value) of all the attributes present inside a node]
		* @param node [parent node]
		* @return attributes [QMap<string, string> of attributes from parent node]
		*/
        QMap<string, string> getAttributes(xml_node &node);
        void addChildren(xml_node args, char* nodeName, QStringList cliArguments);

        template <typename T> 
        xml_node addNode(T &doc, char* nodeName, char* nodeValue = 0);

        template <typename T>
        void addAttribute(xml_node &node, char* attrName, T value);

        void checkErrors(xml_parse_result &result, char* source);
};

#endif //PARSEOPTIONS_H
