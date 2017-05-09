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

		/**
		* [load single parent node of a given file]
		* @param filename [xml file to be loaded]
		* @param nodeName [name of the parent node to be loaded]
        * @return node [parent node]
		*/
        xml_node loadXMLNode(char* filename, char* nodeName);

		/**
		* [get all the children of a node]
		* @param node [parent node]
        * @return nodeChildren [vector of children nodes of parent node]
		*/
        vector<xml_node> getChildren(xml_node &node);

		/**
		* [get particular child of a node]
		* @param node [parent node]
		* @param childName [name of child]
        * @return nodeChild [child node]
		*/
        xml_node getChild(xml_node &node, char* childName);

		/**
		* [get map (name, value) of all the attributes present inside a node]
		* @param node [parent node]
		* @return attributes [QMap<string, string> of attributes from parent node]
		*/
        QMap<string, string> getAttributes(xml_node &node);
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