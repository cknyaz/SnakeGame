#include <QCoreApplication>
#include <SgeCore.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    SgeCore core;
    core.start();

    return app.exec();
}
