#include "parseOptions.h"

ParseOptions::ParseOptions() {

}

ParseOptions::~ParseOptions() {

}

void ParseOptions::createXMLFile() {


    xml_document doc;

    //test
    xml_node args = addNode(doc, "Arguments");


    doc.save_file("save_file_output.xml");

}

xml_node ParseOptions::addNode(xml_document &doc, char* nodeName, char* nodeValue) {

    xml_node node;

    node = doc.append_child(nodeName);

    if (nodeValue) {
        node.append_child(pugi::node_pcdata).set_value(nodeValue);
    }

    return node;
}

void ParseOptions::checkErrors(xml_parse_result &result, char* source) {

    if (result)
    {
        cout << "XML [" << source << "] parsed without errors\n\n";
    }
    else
    {
        cout << "XML [" << source << "] parsed with errors, attr value:\n";
        cout << "Error description: " << result.description() << "\n";
        cout << "Error offset: " << result.offset << " (error at [..." << (source + result.offset) << "]\n\n";
    }

}
