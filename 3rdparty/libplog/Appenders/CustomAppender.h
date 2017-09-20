#ifndef CUSTOMAPPENDER_H
#define CUSTOMAPPENDER_H
#pragma once //
// CustomAppender - shows how to implement a custom appender that stores log messages in memory.
//

#include "Log.h"
#include "Formatters/FuncMessageFormatter.h"
#include <vector>

namespace plog
{
    template<class Formatter> // Typically a formatter is passed as a template parameter.
    class MyAppender : public IAppender // All appenders MUST inherit IAppender interface.
    {
    public:
        virtual void write(const Record& record) // This is a method from IAppender that MUST be implemented.
        {
            util::nstring str = Formatter::format(record); // Use the formatter to get a string from a record.

            if(m_messageList.size()>500) m_messageList.pop_back();

            m_messageList.push_front(str); // Store a log message in a list.
            #ifdef _WIN32
                m_messageQList += QString::fromStdWString(str);
            #else
                m_messageQList += QString::fromStdString(str);
            #endif

        }

        std::list<util::nstring>& getMessageList()
        {
            return m_messageList;
        }

        QString getMessageQList()
        {
            return m_messageQList;
        }

    private:
        std::list<util::nstring> m_messageList;
        QString m_messageQList;
    };
}

#endif // CUSTOMAPPENDER_H