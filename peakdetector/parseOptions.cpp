#include "parseOptions.h"

ParseOptions::ParseOptions(Arguments &arguments) {

    createXMLFile(arguments);
}

ParseOptions::~ParseOptions() {

}

void ParseOptions::createXMLFile(Arguments &arguments) {

    arguments.populateArgs();
    QStringList optionsDialog = arguments.optionsDialogArgs;
    QStringList peakDialog = arguments.peakDialogArgs;
    QStringList general = arguments.generalArgs;


    xml_document doc;
    xml_node args = addNode(doc, "Arguments");

    addChildren(args, "OptionsDialogArguments", optionsDialog);
    addChildren(args, "PeaksDialogArguments", peakDialog);
    addChildren(args, "GeneralArguments", general);

    saveDoc(doc, "test.xml");

}

void ParseOptions::addChildren(xml_node args, char* nodeName, QStringList cliArguments) {

    xml_node node = addNode(args, nodeName);

    for(int i = 0;i < cliArguments.size();i = i + 3) {

        char* ArgName = qStringtocharPointer(cliArguments[i + 1]);
        string strType = cliArguments[i].toStdString();
        char* Type = qStringtocharPointer(cliArguments[i]);

        QString nodeValue = cliArguments[i + 2];
        xml_node nodeArg = addNode(node, ArgName);
        addAttribute(nodeArg, "type", Type);

        if (strType == "int") {
            int Value = nodeValue.toInt();
            addAttribute(nodeArg, "value", Value);
        } else if (strType == "float") {
            float Value = nodeValue.toDouble();
            addAttribute(nodeArg, "value", Value);
        } else if (strType == "string") {
            char* Value = qStringtocharPointer(nodeValue);
            addAttribute(nodeArg, "value", Value);
        } else {
            cerr << "Unknown Type" << endl;
        }


    }

}

char* ParseOptions::qStringtocharPointer(QString stringToBeConverted) {

    QByteArray array = stringToBeConverted.toLocal8Bit();
    char* buffer = array.data();

    return buffer;
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
