#ifndef QTESTWIN_H
#define QTESTWIN_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>

#include <opmapwidget.h>
#include "MapWidget.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class QTestWin : public QWidget
{
    Q_OBJECT

public:
    explicit QTestWin(QWidget *parent = 0);

    virtual int setupLayout(void);
    
signals:
    
public slots:
    void actFatchMap(void);
    void moveUAVStateChanged(bool state);

    void mapWidget_zoomChanged(int newZoom);
    void mapWidget_mouseMoveEvent(QMouseEvent *event);

    virtual void timerEvent(QTimerEvent *event);

protected:
    QSettings                   *m_conf;
    MapWidget                   *m_mapWidget;
    QPushButton                 *m_btnFatchMap;
    QCheckBox                   *m_cbMoveUAV;

    mapcontrol::UAVItem         *m_uav;
};

#endif // QTESTWIN_H
