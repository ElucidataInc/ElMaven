/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns <<"as.call"
                   <<"as.character"
                   <<"as.function"
                   <<"as.list"
                   <<"as.name"
                   <<"assign"
                   <<"attr"
                   <<"attributes"
                   <<"baseenv"
                   <<"basename"
                   <<"body"
                   <<"break"
                   <<"browser"
                   <<"debug"
                   <<"dirname"
                   <<"do.call"
                   <<"emptyenv"
                   <<"environment"
                   <<"eval"
                   <<"file.access"
                   <<"file.append"
                   <<"file.choose"
                   <<"file.copy"
                   <<"file.create"
                   <<"file.exists"
                   <<"file.info"
                   <<"file.path"
                   <<"file.remove"
                   <<"file.rename"
                   <<"file.show"
                   <<"for"
                   <<"formals"
                   <<"function"
                   <<"get"
                   <<"is.na"
                   <<"is.nan"
                   <<"match.arg"
                   <<"match.call"
                   <<"match.fun"
                   <<"missing"
                   <<"mode"
                   <<"NA"
                   <<"names"
                   <<"NaN"
                   <<"new.env"
                   <<"next"
                   <<"NextMethod"
                   <<"NULL"
                   <<"on.exit"
                   <<"pairlist"
                   <<"path.expand"
                   <<"proc.time"
                   <<"quote"
                   <<"repeat"
                   <<"stop"
                   <<"storage.mode"
                   <<"substitute"
                   <<"switch"
                   <<"Sys.getenv"
                   <<"Sys.getlocale"
                   <<"Sys.localeconv"
                   <<"Sys.putenv"
                   <<"Sys.putlocale"
                   <<"Sys.time"
                   <<"Sys.timezone"
                   <<"system"
                   <<"system.time"
                   <<"trace"
                   <<"traceback"
                   <<"typeof"
                   <<"undebug"
                   <<"unlink"
                   <<"untrace"
                   <<"UseMethod"
                   <<"warning"
                   <<"warnings"
                   <<"while"
                   <<"<-"
                   <<"->"
                   <<"==";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp("\\b"+pattern+"");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }



    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+$\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);


    functionFormat.setFontItalic(false);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_.]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    constantFormat.setFontWeight(QFont::Bold);
    constantFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("[0-9]");
    rule.format = constantFormat;
    highlightingRules.append(rule);


    quotationFormat.setForeground(Qt::darkGreen);
    quotationFormat.setFontItalic(false);
    rule.pattern = QRegExp("[\"\'].*[\"|\']");
    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("#[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    /*
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
    */
}
