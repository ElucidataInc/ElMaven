#include "parseOptions.h"
#include <QString>

ParseOptions::ParseOptions() {

}

ParseOptions::~ParseOptions() {

}


QMap<string, string> ParseOptions::getAttributes(xml_node &node) {

    QMap<string, string> attributes;

    for (xml_attribute attr = node.first_attribute(); attr; attr = attr.next_attribute())
    {
        attributes[attr.name()] = attr.value();
    }

    return attributes;

}

void ParseOptions::addChildren(xml_node args, char* nodeName, QStringList cliArguments) {

    xml_node node = addNode(args, nodeName);

    for(int i = 0;i < cliArguments.size();i = i + 3) {

        string argName = cliArguments[i+1].toStdString();
        string strType = cliArguments[i].toStdString();
        string value = cliArguments[i+2].toStdString();

        xml_node nodeArg = addNode(node, const_cast<char*>(argName.c_str()));
        addAttribute(nodeArg, "type", const_cast<char*>(strType.c_str()));

        if (strType == "int") {
            int Value = stoi(value);
            addAttribute(nodeArg, "value", Value);
        }
        else if (strType == "float") {
            float Value = stof(value);
            addAttribute(nodeArg, "value", Value);
        }
        else if (strType == "string") {
            addAttribute(nodeArg, "value", const_cast<char*>(value.c_str()));
        }
        else
            cerr << "Unknown Type" << endl;
    }

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
