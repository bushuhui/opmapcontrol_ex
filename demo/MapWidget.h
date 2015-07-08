#ifndef __MAP_WIDGET_H__
#define __MAP_WIDGET_H__

#include <QtCore>
#include <QtGui>

#include <opmapwidget.h>

#include "UAS_types.h"

using namespace mapcontrol;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MapWidget : public OPMapWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = 0);
    virtual ~MapWidget();

    void setConf(QSettings *conf);
    void syncConf(void);

    void setHome(internals::PointLatLng &p, double alt);
    void getHome(internals::PointLatLng &p, double &alt);

    int  getWaypoints(AP_WPArray &wpa);
    int  setWaypoints(AP_WPArray &wpa);

    void setFlightHeight(double h) {
        m_flightHeight = h;
    }

protected:
    int         setupMenu(void);

protected:
    QSettings   *m_conf;

    QMenu       *m_popupMenu;

    QAction     *m_actMapAccess_ServerAndCache;
    QAction     *m_actMapAccess_Cache;

    QAction     *m_actMapType;

    QAction     *m_actWaypoint_add;
    QAction     *m_actWaypoint_del;
    QAction     *m_actWaypoint_edit;
    QAction     *m_actWaypoint_clear;
    QAction     *m_actWaypoint_save;
    QAction     *m_actWaypoint_load;

    QAction     *m_actSelectArea_beg;
    QAction     *m_actSelectArea_end;
    QAction     *m_actSelectArea_clear;
    int         m_bSelectArea;
    internals::PointLatLng  m_pSelectArea1, m_pSelectArea2;

    QAction     *m_actHome_Set;
    QAction     *m_actHome_Safearea;
    QAction     *m_actHome_ShowHide;

    int                     m_homeShow;             // show or hide home
    double                  m_homeAlt;              // home altitude
    internals::PointLatLng  m_homePos;              // home position
    double                  m_homeSafearea;         // safe area

    double                  m_flightHeight;         // default flight height

signals:
    void        HomeSet(void);

public slots:
    void        actMapAccess_ServerAndCache(void);
    void        actMapAccess_Cache(void);

    void        actMapType_SelectMap(void);

    void        actWaypoint_add(void);
    void        actWaypoint_del(void);
    void        actWaypoint_edit(void);
    void        actWaypoint_clear(void);
    void        actWPEdit(int num, WayPointItem *wp);
    void        actWaypoint_save(void);
    void        actWaypoint_load(void);

    void        actSelectArea_beg(void);
    void        actSelectArea_end(void);
    void        actSelectArea_clear(void);

    void        actHome_Set(void);
    void        actHome_Safearea(void);
    void        actHome_ShowHide(void);

protected:
    void mousePressEvent(QMouseEvent *event);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MapType_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapType_Dialog(QWidget *parent = 0);

    void setupUi(void);

    void setupMapType_list(void);

    void setMapType(core::MapType::Types t);
    core::MapType::Types getMapType(void);

public:
    QDialogButtonBox        *buttonBox;
    QLabel                  *labMapType;
    QComboBox               *cbMapType;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class WaypointEdit_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaypointEdit_Dialog(QWidget *parent = 0);

    void setupUi(void);


    int setWaypoints(int idx,
                     QMap<int, mapcontrol::WayPointItem*> *wpMap,
                     int heightAltitude = 1);
    int setWaypoints_(int idx,
                      QMap<int, mapcontrol::WayPointItem*> *wpMap,
                      int heightAltitude);

    int setTableItem(int ri, int ci, QString s);
    int updateWaypoints(void);

    void setReferenceAltitude(double alt);
    double getReferenceAltitude(void);

protected slots:
    void act_cbAllWaypoints_clicked(bool s);
    void act_cbHeightAltitude_clicked(bool s);

private:
    QVBoxLayout         *verticalLayout;
    QTableWidget        *tableWaypoints;
    QCheckBox           *cbAllWaypoints;
    QCheckBox           *cbHeightAltitude;
    QDialogButtonBox    *buttonBox;

    QMap<int, mapcontrol::WayPointItem*>    *m_wpMap;       // waypoints map
    int                                     m_wpIdx;        // active waypoint index

    int                 m_bHeightAltitude;                  // height or altitude
    double              m_referenceAltitude;                // home altitude (reference)

    QColor              clCL1, clCL2;
    QColor              clB1, clB2;
    int                 fontSize;
    int                 rowHeight;
};


#endif // end of __MAP_WIDGET_H__
