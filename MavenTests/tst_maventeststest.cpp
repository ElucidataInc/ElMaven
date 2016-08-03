#include <QString>
#include <QtTest>

class MavenTestsTest : public QObject
{
Q_OBJECT

public:
MavenTestsTest();

private Q_SLOTS:
void testCase();
};

MavenTestsTest::MavenTestsTest()
{
}

void MavenTestsTest::testCase()
{
        QString str = "Hello";
        QVERIFY(str.toUpper() == "HELLO");
}

QTEST_APPLESS_MAIN(MavenTestsTest)

#include "tst_maventeststest.moc"
