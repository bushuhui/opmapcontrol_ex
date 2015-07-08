
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "MapWidget_TestWindow.h"

QTestWin::QTestWin(QWidget *parent) :
    QWidget(parent)
{
    // create configure obj
    m_conf = new QSettings("./data/demo.ini", QSettings::IniFormat);

    // setup layout
    setupLayout();

    // set window title
    setWindowTitle("MapWidget demo");
}

int QTestWin::setupLayout(void)
{
    // create OPMapWidget
    m_mapWidget = new MapWidget(this);

    double  lat, lng;
    int     zoom;

    lat  = m_conf->value("lastPos_lat", 34.257287).toDouble();
    lng  = m_conf->value("lastPos_lng", 108.888931).toDouble();
    zoom = m_conf->value("lastZoom", 11).toInt();

    internals::PointLatLng p(lat, lng);
    m_mapWidget->SetCurrentPosition(p);
    m_mapWidget->SetZoom(zoom);
    m_mapWidget->setConf(m_conf);

    connect(m_mapWidget, SIGNAL(zoomChanged(int)),
            this, SLOT(mapWidget_zoomChanged(int)));
    connect(m_mapWidget, SIGNAL(mouseMove(QMouseEvent*)),
            this, SLOT(mapWidget_mouseMoveEvent(QMouseEvent*)));

    // set default UAV item
    m_uav = NULL;

    // create right pannel
    QWidget *wLeftPanel = new QWidget(this);
    wLeftPanel->setMinimumWidth(120);

    m_btnFatchMap = new QPushButton(wLeftPanel);
    m_btnFatchMap->setText("Cache map");
    m_btnFatchMap->setGeometry(10, 10, 90, 30);
    connect(m_btnFatchMap, SIGNAL(clicked()), this, SLOT(actFatchMap()));

    m_cbMoveUAV = new QCheckBox("Move UAV", wLeftPanel);
    m_cbMoveUAV->setGeometry(10, 60, 90, 30);
    connect(m_cbMoveUAV, SIGNAL(clicked(bool)), this, SLOT(moveUAVStateChanged(bool)));


    // overall layout
    QHBoxLayout *hl = new QHBoxLayout(this);
    this->setLayout(hl);

    hl->addWidget(m_mapWidget, 1);
    hl->addWidget(wLeftPanel, 0);

    this->startTimer(30);

    return 0;
}


void QTestWin::actFatchMap(void)
{
    internals::RectLatLng  rect;

    rect = m_mapWidget->SelectedArea();
    if( rect.IsEmpty() ) {
        QMessageBox msgbox(QMessageBox::Warning,
                           "Warning", "Please select an area first!");
        msgbox.exec();

        return;
    }

    m_mapWidget->RipMap();
}


void QTestWin::moveUAVStateChanged(bool state)
{
    if( m_cbMoveUAV->checkState() == Qt::Checked ) {
        m_uav = m_mapWidget->AddUAV(0);
    } else {
        m_mapWidget->DeleteUAV(0);
        m_uav = NULL;
    }
}

void QTestWin::mapWidget_zoomChanged(int newZoom)
{
    printf(">>> zoom to: %d\n", newZoom);
}

void QTestWin::mapWidget_mouseMoveEvent(QMouseEvent *event)
{
    if( m_cbMoveUAV->checkState() == Qt::Checked ) {
        internals::PointLatLng p;

        p = m_mapWidget->currentMousePosition();
        printf("p  = %f %f\n", p.Lat(), p.Lng());

        if( m_uav != NULL ) m_uav->SetUAVPos(p, 460);
    }
}

void QTestWin::timerEvent(QTimerEvent *event)
{

}
