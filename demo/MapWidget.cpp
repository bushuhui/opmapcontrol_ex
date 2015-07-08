
#include <QtCore>
#include <QtGui>
#include <QMenu>

#include "UAS_types.h"
#include "MapWidget.h"

using namespace mapcontrol;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct MapType_Data
{
    char    name[256];
    int     typeID;
};

#define MAPTYPE_STRUCT(n)   { #n, core::MapType::n }

static MapType_Data g_arrMapType[] =
{
    MAPTYPE_STRUCT(GoogleMap),
    MAPTYPE_STRUCT(GoogleSatellite),
    MAPTYPE_STRUCT(GoogleLabels),
    MAPTYPE_STRUCT(GoogleTerrain),
    MAPTYPE_STRUCT(GoogleHybrid),

    MAPTYPE_STRUCT(GoogleMapChina),
    MAPTYPE_STRUCT(GoogleSatelliteChina),
    MAPTYPE_STRUCT(GoogleLabelsChina),
    MAPTYPE_STRUCT(GoogleTerrainChina),
    MAPTYPE_STRUCT(GoogleHybridChina),

    MAPTYPE_STRUCT(OpenStreetMap),
    MAPTYPE_STRUCT(OpenStreetOsm),
    MAPTYPE_STRUCT(OpenStreetMapSurfer),
    MAPTYPE_STRUCT(OpenStreetMapSurferTerrain),

    MAPTYPE_STRUCT(YahooMap),
    MAPTYPE_STRUCT(YahooSatellite),
    MAPTYPE_STRUCT(YahooLabels),
    MAPTYPE_STRUCT(YahooHybrid),

    MAPTYPE_STRUCT(BingMap),
    MAPTYPE_STRUCT(BingSatellite),
    MAPTYPE_STRUCT(BingHybrid),

    MAPTYPE_STRUCT(ArcGIS_Map),
    MAPTYPE_STRUCT(ArcGIS_Satellite),
    MAPTYPE_STRUCT(ArcGIS_ShadedRelief),
    MAPTYPE_STRUCT(ArcGIS_Terrain),
    MAPTYPE_STRUCT(ArcGIS_WorldTopo),

    MAPTYPE_STRUCT(ArcGIS_MapsLT_Map),
    MAPTYPE_STRUCT(ArcGIS_MapsLT_OrtoFoto),
    MAPTYPE_STRUCT(ArcGIS_MapsLT_Map_Labels),
    MAPTYPE_STRUCT(ArcGIS_MapsLT_Map_Hybrid),

    MAPTYPE_STRUCT(PergoTurkeyMap),
    MAPTYPE_STRUCT(SigPacSpainMap),

    MAPTYPE_STRUCT(GoogleMapKorea),
    MAPTYPE_STRUCT(GoogleSatelliteKorea),
    MAPTYPE_STRUCT(GoogleLabelsKorea),
    MAPTYPE_STRUCT(GoogleHybridKorea),

    MAPTYPE_STRUCT(YandexMapRu),
    MAPTYPE_STRUCT(Statkart_Topo2),

    {"NULL", -1}
};

char *getMapName_fromID(core::MapType::Types t)
{
    int     i = 0;

    while(1) {
        if( g_arrMapType[i].typeID == t )
            return g_arrMapType[i].name;

        if( g_arrMapType[i].typeID < 0 ) return NULL;

        i ++;
    }

    return NULL;
}


MapType_Dialog::MapType_Dialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle("Select Map Type");

    setupUi();

    setupMapType_list();
    setMapType(core::MapType::GoogleSatellite);
}

void MapType_Dialog::setupUi(void)
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("MapWidget_MapTypeDiag"));
    this->setFixedSize(374, 122);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(20, 80, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    labMapType = new QLabel(this);
    labMapType->setObjectName(QString::fromUtf8("labMapType"));
    labMapType->setGeometry(QRect(32, 34, 61, 15));
    labMapType->setText("Map Type:");

    cbMapType = new QComboBox(this);
    cbMapType->setObjectName(QString::fromUtf8("cbMapType"));
    cbMapType->setGeometry(QRect(100, 30, 251, 25));

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void MapType_Dialog::setupMapType_list(void)
{
    int     i;

    i = 0;
    while(1) {
        if( g_arrMapType[i].typeID >= 0 )
            cbMapType->addItem(g_arrMapType[i].name);
        else
            break;

        i ++;
    }
}

void MapType_Dialog::setMapType(core::MapType::Types t)
{
    int i = 0;

    while(1) {
        if( g_arrMapType[i].typeID == t ) {
            cbMapType->setCurrentIndex(i);
            return;
        }

        if( g_arrMapType[i].typeID == -1 ) break;

        i ++;
    }

    // set default map
    cbMapType->setCurrentIndex(0);
}

core::MapType::Types MapType_Dialog::getMapType(void)
{
    int     idx, typeID;

    idx    = cbMapType->currentIndex();
    typeID = g_arrMapType[cbMapType->currentIndex()].typeID;

    return (core::MapType::Types) typeID;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

WaypointEdit_Dialog::WaypointEdit_Dialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle("Waypoints Edit");

    m_wpMap  = NULL;
    m_wpIdx  = 0;

    clCL1 = QColor(0x00, 0x00, 0xFF);
    clCL2 = QColor(0x00, 0x00, 0x00);
    clB1  = QColor(0xFF, 0xFF, 0xFF);
    clB2  = QColor(0xE8, 0xE8, 0xE8);

    fontSize  = 10;
    rowHeight = 24;

    m_bHeightAltitude = 1;
    m_referenceAltitude = 440.0;

    setupUi();
}

int WaypointEdit_Dialog::setWaypoints(int idx, QMap<int, mapcontrol::WayPointItem*> *wpMap,
                                      int heightAltitude)
{
    m_wpIdx  = idx;
    m_wpMap  = wpMap;
    m_bHeightAltitude = heightAltitude;

    // set height/altitude checkbox
    if( heightAltitude ) {
        cbHeightAltitude->setCheckState(Qt::Checked);
    } else {
        cbHeightAltitude->setCheckState(Qt::Unchecked);
    }

    // if edit all waypoints then disable checkbox of all waypoints
    if( m_wpIdx < 0 ) cbAllWaypoints->setDisabled(true);

    // set table items
    setWaypoints_(idx, wpMap, heightAltitude);

    return 0;
}

int WaypointEdit_Dialog::setWaypoints_(int idx,
                                       QMap<int, mapcontrol::WayPointItem*> *wpMap,
                                       int heightAltitude)
{
    if( heightAltitude ) {
        tableWaypoints->horizontalHeaderItem(1)->setText("Height");
    } else {
        tableWaypoints->horizontalHeaderItem(1)->setText("Altitude");
    }

    // clear old contents
    tableWaypoints->clearContents();

    // insert items
    double h;

    if( idx >= 0 ) {
        cbAllWaypoints->setCheckState(Qt::Unchecked);
        tableWaypoints->setRowCount(1);

        mapcontrol::WayPointItem *item;
        item = wpMap->value(idx);

        if( heightAltitude ) h = item->Altitude() - m_referenceAltitude;
        else                 h = item->Altitude();

        setTableItem(0, 0, QString("%1").arg(item->Number()));
        setTableItem(0, 1, QString("%1").arg(h));
        setTableItem(0, 2, QString("%1").arg(item->Heading()));

        tableWaypoints->item(0, 0)->setFlags(Qt::ItemIsSelectable);
        tableWaypoints->item(0, 2)->setToolTip("0:North, 90:East, 180:South, 270:West");
    } else {
        cbAllWaypoints->setCheckState(Qt::Checked);

        QList<int> ids = wpMap->keys();
        int        ri = 0;

        tableWaypoints->setRowCount(ids.size());

        foreach(int i, ids) {
            mapcontrol::WayPointItem *item;
            item = wpMap->value(i);

            if( heightAltitude ) h = item->Altitude() - m_referenceAltitude;
            else                 h = item->Altitude();

            setTableItem(ri, 0, QString("%1").arg(item->Number()));
            setTableItem(ri, 1, QString("%1").arg(h));
            setTableItem(ri, 2, QString("%1").arg(item->Heading()));

            tableWaypoints->item(ri, 0)->setFlags(Qt::ItemIsSelectable);
            tableWaypoints->item(ri, 2)->setToolTip("0:North, 90:East, 180:South, 270:West");

            ri++;
        }
    }

    return 0;
}

int WaypointEdit_Dialog::setTableItem(int ri, int ci, QString s)
{
    if( tableWaypoints->item(ri, ci) != NULL ) {
        tableWaypoints->item(ri, ci)->setText(s);
    } else {
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setText(s);

        item->setTextColor(clCL2);
        if( ri % 2 == 0 ) item->setBackgroundColor(clB1);
        else              item->setBackgroundColor(clB2);

        item->setFont(QFont("", fontSize));

        tableWaypoints->setItem(ri, ci, item);
        tableWaypoints->setRowHeight(ri, rowHeight);
    }

    return 0;
}

int WaypointEdit_Dialog::updateWaypoints(void)
{
    int     i, n;
    int     heightAltitude;

    int     idx;
    double  alt, heading;

    if( cbHeightAltitude->checkState() == Qt::Checked )
        heightAltitude = 1;
    else
        heightAltitude = 0;

    n = tableWaypoints->rowCount();
    for(i=0; i<n; i++) {
        idx     = tableWaypoints->item(i, 0)->text().toInt();
        alt     = tableWaypoints->item(i, 1)->text().toDouble();
        heading = tableWaypoints->item(i, 2)->text().toDouble();

        if( heightAltitude ) alt += m_referenceAltitude;

        m_wpMap->value(idx)->SetAltitude(alt);
        m_wpMap->value(idx)->SetHeading(heading);
    }

    return 0;
}

void WaypointEdit_Dialog::setReferenceAltitude(double alt)
{
    m_referenceAltitude = alt;
}

double WaypointEdit_Dialog::getReferenceAltitude(void)
{
    return m_referenceAltitude;
}

void WaypointEdit_Dialog::setupUi(void)
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("Waypoint_EditDialog"));
    this->resize(695, 439);

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    // waypoint edit table
    tableWaypoints = new QTableWidget(this);
    tableWaypoints->setObjectName(QString::fromUtf8("tableWaypoints"));
    tableWaypoints->verticalHeader()->hide();
    tableWaypoints->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    tableWaypoints->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tableWaypoints->setColumnCount(3);
    QStringList slHeader;
    slHeader.append("Index");
    slHeader.append("Altitude");
    slHeader.append("Heading");
    tableWaypoints->setHorizontalHeaderLabels(slHeader);

    // all waypoint checkbox
    cbAllWaypoints = new QCheckBox(this);
    cbAllWaypoints->setObjectName(QString::fromUtf8("cbAllWaypoints"));
    cbAllWaypoints->setText("All Waypoints");
    connect(cbAllWaypoints, SIGNAL(clicked(bool)),
            this, SLOT(act_cbAllWaypoints_clicked(bool)));

    // height / altitude checkbox
    cbHeightAltitude = new QCheckBox(this);
    cbHeightAltitude->setObjectName(QString::fromUtf8("cbHeightAltitude"));
    cbHeightAltitude->setText("Height / Altitude");
    connect(cbHeightAltitude, SIGNAL(clicked(bool)),
            this, SLOT(act_cbHeightAltitude_clicked(bool)));

    // ok/cancle buttonbox
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout->addWidget(tableWaypoints);
    verticalLayout->addWidget(cbAllWaypoints);
    verticalLayout->addWidget(cbHeightAltitude);
    verticalLayout->addWidget(buttonBox);


    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void WaypointEdit_Dialog::act_cbAllWaypoints_clicked(bool s)
{
    if( cbAllWaypoints->checkState() == Qt::Checked ) {
        setWaypoints_(-1, m_wpMap, m_bHeightAltitude);
    } else{
        setWaypoints_(m_wpIdx, m_wpMap, m_bHeightAltitude);
    }
}

void WaypointEdit_Dialog::act_cbHeightAltitude_clicked(bool s)
{
    if( cbHeightAltitude->checkState() == Qt::Checked ) {
        m_bHeightAltitude = 1;
    } else {
        m_bHeightAltitude = 0;
    }

    if( cbAllWaypoints->checkState() == Qt::Checked ) {
        setWaypoints_(-1, m_wpMap, m_bHeightAltitude);
    } else {
        setWaypoints_(m_wpIdx, m_wpMap, m_bHeightAltitude);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

MapWidget::MapWidget(QWidget *parent) :
    mapcontrol::OPMapWidget(parent)
{
    m_conf = NULL;

    configuration->SetAccessMode(core::AccessMode::CacheOnly);
    configuration->SetTileMemorySize(200);
    configuration->SetCacheLocation("./data/");

    SetZoom(4);
    SetMinZoom(4);
    SetMaxZoom(18);
    SetMapType(MapType::GoogleHybrid);

    // set initial values
    m_bSelectArea = 0;
    m_pSelectArea1.SetLat(-9999);   m_pSelectArea1.SetLng(-9999);
    m_pSelectArea2.SetLat(-9999);   m_pSelectArea2.SetLng(-9999);

    m_homeShow = 1;
    m_homeAlt = 440;
    m_homePos.SetLat(-9999);        m_homePos.SetLng(-9999);
    m_homeSafearea = 100;

    m_flightHeight = 20;

    // setup menus
    setupMenu();
}

MapWidget::~MapWidget()
{

}

void MapWidget::setConf(QSettings *conf)
{
    m_conf = conf;

    if( m_conf == NULL ) return;

    // map type & access mode
    {
        MapType::Types              mapType;
        core::AccessMode::Types     accessMode;
        QString                     cacheLocation;

        // load settings
        accessMode    = (core::AccessMode::Types) m_conf->value("mapWidget_accessMode",
                                                                (int)(core::AccessMode::CacheOnly)).toInt();
        mapType       = (MapType::Types) m_conf->value("mapWidget_mapType",
                                                       (int)(MapType::GoogleSatellite)).toInt();
        cacheLocation = m_conf->value("mapWidget_cacheLocation", "./data/").toString();

        // set configurations
        configuration->SetAccessMode(accessMode);
        configuration->SetCacheLocation(cacheLocation);
        SetMapType(mapType);

        // set accessMode actions
        if( accessMode == core::AccessMode::ServerAndCache ) {
            m_actMapAccess_ServerAndCache->setChecked(true);
            m_actMapAccess_Cache->setChecked(false);
        } else if ( accessMode == core::AccessMode::CacheOnly ) {
            m_actMapAccess_ServerAndCache->setChecked(false);
            m_actMapAccess_Cache->setChecked(true);
        }
    }

    // home & safe area
    {
        m_homeShow = m_conf->value("mapWidget_home_show", m_homeShow).toInt();
        m_homePos.SetLat(m_conf->value("mapWidget_home_lat", m_homePos.Lat()).toDouble());
        m_homePos.SetLng(m_conf->value("mapWidget_home_lng", m_homePos.Lng()).toDouble());
        m_homeAlt = m_conf->value("mapWidget_home_alt", m_homeAlt).toDouble();
        m_homeSafearea = m_conf->value("mapWidget_home_safeArea", m_homeSafearea).toDouble();

        m_flightHeight = m_conf->value("mapWidget_flightHeight", m_flightHeight).toDouble();

        if( m_homeShow ) {
            this->SetShowHome(true);
            this->Home->SetCoord(m_homePos);
            this->Home->SetAltitude( (int)(m_homeAlt) );
            this->Home->SetSafeArea((int)(m_homeSafearea));

            m_actHome_ShowHide->setChecked(true);
        } else {
            m_actHome_ShowHide->setChecked(false);
        }
    }
}

void MapWidget::syncConf(void)
{
    if( m_conf == NULL ) return;

    // map type & access mode
    MapType::Types              mapType;
    core::AccessMode::Types     accessMode;

    mapType = GetMapType();
    accessMode = configuration->AccessMode();
    m_conf->setValue("mapWidget_mapType", (int)(mapType));
    m_conf->setValue("mapWidget_accessMode", (int)(accessMode));

    // home
    m_conf->setValue("mapWidget_home_show", m_homeShow);
    m_conf->setValue("mapWidget_home_lat", m_homePos.Lat());
    m_conf->setValue("mapWidget_home_lng", m_homePos.Lng());
    m_conf->setValue("mapWidget_home_alt", m_homeAlt);
    m_conf->setValue("mapWidget_home_safeArea", m_homeSafearea);

    m_conf->setValue("mapWidget_flightHeight", m_flightHeight);

    // sync to configure file
    m_conf->sync();
}

void MapWidget::setHome(internals::PointLatLng &p, double alt)
{
    m_homePos = p;
    m_homeAlt = alt;

    this->SetShowHome(true);
    this->Home->SetCoord(m_homePos);
    this->Home->SetAltitude( (int)(m_homeAlt) );
    this->Home->SetSafeArea((int)(m_homeSafearea));

    syncConf();

    ReloadMap();
}

void MapWidget::getHome(internals::PointLatLng &p, double &alt)
{
    p = m_homePos;
    alt = m_homeAlt;
}


int MapWidget::getWaypoints(AP_WPArray &wpa)
{
    QMap<int, mapcontrol::WayPointItem*> wpMap;

    // get waypoints
    wpMap  = WPAll();

    foreach(mapcontrol::WayPointItem* p, wpMap) {
        AP_WayPoint wp;

        wp.idx      = p->Number();
        wp.lng      = p->Coord().Lng();
        wp.lat      = p->Coord().Lat();
        wp.alt      = p->Altitude();
        wp.heading  = p->Heading();

        wpa.set(wp);
    }

    return 0;
}

int MapWidget::setWaypoints(AP_WPArray &wpa)
{
    return 0;
}

int MapWidget::setupMenu(void)
{
    // setup actions
    m_actMapType = new QAction(tr("Map Type"), this);
    connect(m_actMapType, SIGNAL(triggered()),
            this, SLOT(actMapType_SelectMap()));


    m_actMapAccess_ServerAndCache = new QAction(tr("ServerAndCache"), this);
    m_actMapAccess_ServerAndCache->setCheckable(true);
    m_actMapAccess_ServerAndCache->setChecked(false);
    connect(m_actMapAccess_ServerAndCache, SIGNAL(triggered()),
            this, SLOT(actMapAccess_ServerAndCache()));

    m_actMapAccess_Cache = new QAction(tr("Cache"), this);
    m_actMapAccess_Cache->setCheckable(true);
    m_actMapAccess_Cache->setChecked(true);
    connect(m_actMapAccess_Cache, SIGNAL(triggered()),
            this, SLOT(actMapAccess_Cache()));


    m_actWaypoint_add = new QAction(tr("Waypoint Add"), this);
    connect(m_actWaypoint_add, SIGNAL(triggered()),
            this, SLOT(actWaypoint_add()));
    m_actWaypoint_del = new QAction(tr("Waypoint Delete"), this);
    connect(m_actWaypoint_del, SIGNAL(triggered()),
            this, SLOT(actWaypoint_del()));
    m_actWaypoint_edit = new QAction(tr("Waypoint Edit"), this);
    connect(m_actWaypoint_edit, SIGNAL(triggered()),
            this, SLOT(actWaypoint_edit()));
    m_actWaypoint_clear = new QAction(tr("Waypoint Clear"), this);
    connect(m_actWaypoint_clear, SIGNAL(triggered()),
            this, SLOT(actWaypoint_clear()));
    m_actWaypoint_save = new QAction(tr("Waypoint Save"), this);
    connect(m_actWaypoint_save, SIGNAL(triggered()),
            this, SLOT(actWaypoint_save()));
    m_actWaypoint_load = new QAction(tr("Waypoint Load"), this);
    connect(m_actWaypoint_load, SIGNAL(triggered()),
            this, SLOT(actWaypoint_load()));

    m_actSelectArea_beg = new QAction(tr("SelectArea Begin"), this);
    connect(m_actSelectArea_beg, SIGNAL(triggered()),
            this, SLOT(actSelectArea_beg()));
    m_actSelectArea_end = new QAction(tr("SelectArea End"), this);
    connect(m_actSelectArea_end, SIGNAL(triggered()),
            this, SLOT(actSelectArea_end()));
    m_actSelectArea_clear = new QAction(tr("SelectArea Clear"), this);
    connect(m_actSelectArea_clear, SIGNAL(triggered()),
            this, SLOT(actSelectArea_clear()));

    m_actHome_Set = new QAction(tr("Set Home"), this);
    connect(m_actHome_Set, SIGNAL(triggered()),
            this, SLOT(actHome_Set()));
    m_actHome_Safearea = new QAction(tr("Set Home Safearea"), this);
    connect(m_actHome_Safearea, SIGNAL(triggered()),
            this, SLOT(actHome_Safearea()));
    m_actHome_ShowHide = new QAction(tr("Show/Hide Home"), this);
    m_actHome_ShowHide->setCheckable(true);
    m_actHome_ShowHide->setChecked(true);
    connect(m_actHome_ShowHide, SIGNAL(triggered()),
            this, SLOT(actHome_ShowHide()));

    // setup menu
    m_popupMenu = new QMenu("Menu");

    QMenu *menuAccessMode = m_popupMenu->addMenu("Access Type");
    m_popupMenu->addAction(m_actMapType);
    m_popupMenu->addSeparator();
    m_popupMenu->addAction(m_actWaypoint_add);
    m_popupMenu->addAction(m_actWaypoint_del);
    m_popupMenu->addAction(m_actWaypoint_edit);
    m_popupMenu->addAction(m_actWaypoint_clear);
    m_popupMenu->addAction(m_actWaypoint_save);
    m_popupMenu->addAction(m_actWaypoint_load);
    m_popupMenu->addSeparator();
    m_popupMenu->addAction(m_actSelectArea_beg);
    m_popupMenu->addAction(m_actSelectArea_end);
    m_popupMenu->addAction(m_actSelectArea_clear);
    m_popupMenu->addSeparator();
    m_popupMenu->addAction(m_actHome_Set);
    m_popupMenu->addAction(m_actHome_Safearea);
    m_popupMenu->addAction(m_actHome_ShowHide);

    menuAccessMode->addAction(m_actMapAccess_ServerAndCache);
    menuAccessMode->addAction(m_actMapAccess_Cache);

    return 0;
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if( event->button() == Qt::RightButton ) {
        m_popupMenu->popup(event->globalPos());
    } else {
        mapcontrol::OPMapWidget::mousePressEvent(event);
    }
}


void MapWidget::actMapType_SelectMap(void)
{
    MapType_Dialog          diag;
    core::MapType::Types    mt, mt0;

    // set current map type active
    mt0 = GetMapType();
    diag.setMapType(mt0);

    // open maptype selecting dialog
    if( QDialog::Accepted == diag.exec() ) {
        mt = diag.getMapType();

        if( mt0 == mt ) return;

        // set new map type
        SetMapType(mt);

        // update settings
        if( m_conf != NULL ) {
            m_conf->setValue("mapWidget_mapType", (int)(mt));
            m_conf->sync();
        }

        // reload map
        ReloadMap();
    }
}


void MapWidget::actMapAccess_ServerAndCache(void)
{
    m_actMapAccess_ServerAndCache->setChecked(true);
    m_actMapAccess_Cache->setChecked(false);

    configuration->SetAccessMode(core::AccessMode::ServerAndCache);

    if( m_conf != NULL ) {
        m_conf->setValue("mapWidget_accessMode", (int)(core::AccessMode::ServerAndCache));
        m_conf->sync();
    }
}

void MapWidget::actMapAccess_Cache(void)
{
    m_actMapAccess_ServerAndCache->setChecked(false);
    m_actMapAccess_Cache->setChecked(true);

    configuration->SetAccessMode(core::AccessMode::CacheOnly);

    if( m_conf != NULL ) {
        m_conf->setValue("mapWidget_accessMode", (int)(core::AccessMode::CacheOnly));
        m_conf->sync();
    }
}


void MapWidget::actWaypoint_add(void)
{
    internals::PointLatLng p;

    p = currentMousePosition();

    mapcontrol::WayPointItem *wp = this->WPCreate();
    wp->SetCoord(p);
    wp->SetAltitude(m_homeAlt + m_flightHeight);

    connect(wp, SIGNAL(WPEdit(int, WayPointItem*)),
            this, SLOT(actWPEdit(int, WayPointItem*)));
}

void MapWidget::actWaypoint_del(void)
{
    QList<mapcontrol::WayPointItem*>    wpList;
    int                     i, n;

    // set selected waypoints
    wpList = WPSelected();
    n = wpList.size();

    for(i=0; i<n; i++) {
        WPDelete(wpList[i]);
    }
}

void MapWidget::actWaypoint_edit(void)
{
    QMap<int, mapcontrol::WayPointItem*>    wpMap;
    QList<mapcontrol::WayPointItem*>        wpList;
    int                                     idx;

    WaypointEdit_Dialog                     wpDialog;

    wpMap  = WPAll();
    wpList = WPSelected();

    wpDialog.setReferenceAltitude(m_homeAlt);

    if( wpList.size() == 1 ) {
        idx = wpList[0]->Number();

        wpDialog.setWaypoints(idx, &wpMap);
    } else {
        wpDialog.setWaypoints(-1, &wpMap);
    }

    // update edited data
    if( QDialog::Accepted == wpDialog.exec() ) {
        wpDialog.updateWaypoints();
    }
}

void MapWidget::actWaypoint_clear(void)
{
    WPDeleteAll();
    ReloadMap();
}

void MapWidget::actWPEdit(int num, WayPointItem *wp)
{
    QMap<int, mapcontrol::WayPointItem*>    wpMap;
    int                                     idx;

    WaypointEdit_Dialog                     wpDialog;

    wpMap  = WPAll();
    idx    = num;

    wpDialog.setReferenceAltitude(m_homeAlt);
    wpDialog.setWaypoints(idx, &wpMap);

    if( QDialog::Accepted == wpDialog.exec() ) {
        wpDialog.updateWaypoints();
    }
}


void MapWidget::actWaypoint_save(void)
{
    QMap<int, mapcontrol::WayPointItem*> wpMap;
    QString fname, path = "./data";

    // get waypoints
    wpMap  = WPAll();

    // get save filename
    fname = QFileDialog::getSaveFileName(this, tr("Waypoint file"),
                                         path,
                                         tr("Waypoint Files (*.wp)"));

    if( fname.size() < 1 ) return;
    if( !fname.endsWith(".wp", Qt::CaseInsensitive) ) {
        fname = fname + ".wp";
    }

    // copy to AP_WPArray
    AP_WPArray  arrWP;

    foreach(mapcontrol::WayPointItem* p, wpMap) {
        AP_WayPoint wp;

        wp.idx = p->Number();
        wp.lng = p->Coord().Lng();
        wp.lat = p->Coord().Lat();
        wp.alt = p->Altitude();
        wp.heading = p->Heading();

        arrWP.set(wp);
    }

    // save to file
    arrWP.save(fname.toStdString());
}

void MapWidget::actWaypoint_load(void)
{
    QString fname, path = "./data";

    AP_WPArray                  arrWP;
    AP_WayPointMap              *wpMap;
    AP_WayPointMap::iterator    it;
    AP_WayPoint                 *p;

    // get save filename
    fname = QFileDialog::getOpenFileName(this, tr("Waypoint file"),
                                         path,
                                         tr("Waypoint Files (*.wp)"));

    if( fname.size() < 1 ) return;

    // load AP_WPArray
    if( 0 != arrWP.load(fname.toStdString()) ) return;

    // delete exist waypoints
    this->WPDeleteAll();

    // insert to map widget
    wpMap = arrWP.getAll();

    for(it=wpMap->begin(); it!=wpMap->end(); it++) {
        p = it->second;

        // create new wp item
        internals::PointLatLng coord(p->lat, p->lng);
        mapcontrol::WayPointItem* wp = new mapcontrol::WayPointItem(coord, p->alt, map);
        wp->SetNumber(p->idx);
        wp->SetHeading(p->heading);

        // insert to widget
        WPCreate(wp);
    }

    // repaint map
    ReloadMap();
}


void MapWidget::actSelectArea_beg(void)
{
    internals::PointLatLng p;

    p = currentMousePosition();

    m_pSelectArea1.SetLat(p.Lat());
    m_pSelectArea1.SetLng(p.Lng());
}

void MapWidget::actSelectArea_end(void)
{
    internals::PointLatLng p;

    double  lat1, lat2;
    double  lng1, lng2;

    p = currentMousePosition();

    m_pSelectArea2.SetLat(p.Lat());
    m_pSelectArea2.SetLng(p.Lng());

    if( m_pSelectArea1.Lat() < -1000 || m_pSelectArea1.Lng() < -1000 )
        return;

    if( m_pSelectArea1.Lat() > m_pSelectArea2.Lat() ) {
        lat1 = m_pSelectArea1.Lat();
        lat2 = m_pSelectArea2.Lat();
    } else {
        lat1 = m_pSelectArea2.Lat();
        lat2 = m_pSelectArea1.Lat();
    }

    if( m_pSelectArea1.Lng() < m_pSelectArea2.Lng() ) {
        lng1 = m_pSelectArea1.Lng();
        lng2 = m_pSelectArea2.Lng();
    } else {
        lng1 = m_pSelectArea2.Lng();
        lng2 = m_pSelectArea1.Lng();
    }


    internals::RectLatLng rect(lat1, lng1, fabs(lng2 - lng1), fabs(lat2 - lat1));
    SetSelectedArea(rect);

    //this->update();
    ReloadMap();
}

void MapWidget::actSelectArea_clear(void)
{
    internals::RectLatLng rect;

    SetSelectedArea(rect);

    //this->update();
    ReloadMap();
}

void MapWidget::actHome_Set(void)
{
    internals::PointLatLng p;

    p = currentMousePosition();

    // get home altitude
    bool ok = true;
    double alt = QInputDialog::getDouble(this,
                                         tr("Input altitude of home"),
                                         tr("Altitude:"),
                                         m_homeAlt, -1000, 10000,
                                         1,
                                         &ok);
    if ( ok ) m_homeAlt = alt;
    else return;

    // set home position, altitude, safearea
    this->SetShowHome(true);
    this->Home->SetCoord(p);
    this->Home->SetAltitude((int)(m_homeAlt));
    this->Home->SetSafeArea((int)(m_homeSafearea));
    m_homePos = p;

    // write configure
    syncConf();

    // refresh map
    ReloadMap();

    // send home set signal
    emit HomeSet();
}

void MapWidget::actHome_Safearea(void)
{
    bool ok = true;
    double d = QInputDialog::getDouble(this,
                                       tr("Set safe area"),
                                       tr("Radius (m):"),
                                       m_homeSafearea, 20, 50000,
                                       1,
                                       &ok);
    if ( ok ) m_homeSafearea = d;
    else return;

    // write configure
    syncConf();

    if( Home != NULL ) {
        this->Home->SetSafeArea((int)(m_homeSafearea));

        // refresh map
        ReloadMap();
    }
}

void MapWidget::actHome_ShowHide(void)
{
    if( m_homeShow ) m_homeShow = 0;
    else             m_homeShow = 1;

    if( m_homeShow ) {
        setHome(m_homePos, m_homeAlt);

        m_actHome_ShowHide->setChecked(true);
    } else {
        this->SetShowHome(false);

        m_actHome_ShowHide->setChecked(false);
    }

    // write configure
    syncConf();
}

