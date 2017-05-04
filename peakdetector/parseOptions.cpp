#include "parseOptions.h"

ParseOptions::ParseOptions(const Arguments &args) {


}

ParseOptions::~ParseOptions() {

}

void ParseOptions::createXMLFile() {


    xml_document doc;

    //test
    xml_node args = addNode(doc, "Arguments");

    xml_node child = addNode(args, "OptionsTab", "Arguments/Variables from Options Tab in ElMaven GUI");

    saveDoc(doc, "test.xml");

}

template <typename T> 
xml_node ParseOptions::addNode(T &doc, char* nodeName, char* nodeValue) {

    xml_node node;

    node = doc.append_child(nodeName);

    if (nodeValue) {
        node.append_child(pugi::node_pcdata).set_value(nodeValue);
    }

    return node;
}

template <typename T>
void ParseOptions::addAttribute(xml_node &node, char* attrName, T value) {

    node.append_attribute(attrName) = value;

}

void ParseOptions::saveDoc(xml_document &doc, char* docPath) {

    doc.save_file(docPath);

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
