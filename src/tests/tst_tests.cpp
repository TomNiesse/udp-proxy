#include <QCoreApplication>
#include <QTest>

// add necessary includes here

class tests : public QObject
{
    Q_OBJECT

public:
    tests();
    ~tests() override;

private slots:
    void test_case1();
};

tests::tests() {}

tests::~tests() = default;

void tests::test_case1() {}

QTEST_MAIN(tests)

#include "tst_tests.moc"
