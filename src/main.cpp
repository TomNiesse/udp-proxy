#include "app.h"
#include "version.h"

int main(int argc, char *argv[])
{
    App app(argc, argv);

    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

    app.parseCommandLineOptions();
    app.initialize();

    return app.exec();
}
