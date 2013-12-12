#include "MainWindow.hpp"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("crumblingstatue");
    a.setApplicationName("NoLifeExplorer");
    MainWindow w;
    w.show();
    return a.exec();
}
