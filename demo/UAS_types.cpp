
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>

#include "UAS_types.h"


using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

string trim(const string &s)
{
    string              delims = " \t\n\r",
                        r;
    string::size_type   i, j;

    i = s.find_first_not_of(delims);
    j = s.find_last_not_of(delims);

    if( i == string::npos ) {
        r = "";
        return r;
    }

    if( j == string::npos ) {
        r = "";
        return r;
    }

    r = s.substr(i, j-i+1);
    return r;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

AP_ParamItem::AP_ParamItem()
{
    init();
}

AP_ParamItem::~AP_ParamItem()
{
    release();
}

void AP_ParamItem::init()
{
    index = 0;
    memset(id, 0, 17);
    type = 9;
    value = 0;

    modified = 0;
}

void AP_ParamItem::release()
{
    index = 0;
    memset(id, 0, 17);
    type = 9;
    value = 0;

    modified = 0;
}

int8_t      AP_ParamItem::toInt8(void)
{
    return (int8_t) value;
}

uint8_t     AP_ParamItem::toUint8(void)
{
    return (uint8_t) value;
}

int16_t     AP_ParamItem::toInt16(void)
{
    return (int16_t) value;
}

uint16_t    AP_ParamItem::toUint16(void)
{
    return (uint16_t) value;
}

int32_t     AP_ParamItem::toInt32(void)
{
    return (int32_t) value;
}

uint32_t    AP_ParamItem::toUint32(void)
{
    return (uint32_t) value;
}

float       AP_ParamItem::toFloat(void)
{
    return value;
}

void        AP_ParamItem::fromInt8(int8_t v)
{
    value = v;
}

void        AP_ParamItem::fromUint8(uint8_t v)
{
    value = v;
}

void        AP_ParamItem::fromInt16(int16_t v)
{
    value = v;
}

void        AP_ParamItem::fromUint16(uint16_t v)
{
    value = v;
}

void        AP_ParamItem::fromInt32(int32_t v)
{
    value = v;
}

void        AP_ParamItem::fromUint32(uint32_t v)
{
    value = v;
}

void        AP_ParamItem::fromFloat(float v)
{
    value = v;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

AP_ParamArray::AP_ParamArray()
{
    init();
}

AP_ParamArray::~AP_ParamArray()
{
    release();
}

void AP_ParamArray::init(void)
{
    m_nParam = 0;
    m_idxCurrent = 0;

    m_lstAll.clear();
    m_mapIndex.clear();
    m_mapID.clear();

    m_stParamReading = IDLE;
    m_tLastReading = 0;

    m_bLoaded = 0;
}

void AP_ParamArray::release(void)
{
    std::vector<AP_ParamItem*>::iterator it;

    for(it=m_lstAll.begin(); it!=m_lstAll.end(); it++) {
        delete *it;
    }
    m_lstAll.clear();

    m_mapIndex.clear();
    m_mapID.clear();

    m_bLoaded = 0;
}

int AP_ParamArray::clear(void)
{
    release();

    m_nParam = 0;
    m_idxCurrent = 0;

    m_stParamReading = IDLE;
    m_tLastReading = 0;

    return 0;
}

int AP_ParamArray::lock(void)
{
    return 0;
}

int AP_ParamArray::unlock(void)
{
    return 0;
}


int AP_ParamArray::set(AP_ParamArray &pa)
{
    std::vector<AP_ParamItem*>::iterator it;
    AP_ParamItem *pi, *pi2;

    // copy all items
    for(it=pa.m_lstAll.begin(); it!=pa.m_lstAll.end(); it++) {
        pi = *it;

        pi2 = get(pi->id);
        if( pi2 == NULL ) {
            set(*pi);
        } else {
            pi2->type = pi->type;
            if( pi2->value != pi->value ) {
                pi2->modified = 1;
            }

            pi2->value = pi->value;
        }
    }

    // set flags
    m_stParamReading = FINISHED;
    m_bLoaded = 1;

    return 0;
}

int AP_ParamArray::set(AP_ParamItem &item)
{
    AP_ParamItem    *pi;

    pi = get(item.id);

    m_idxCurrent = item.index;

    if( pi != NULL ) {
        pi->value = item.value;
        pi->modified = item.modified;
    } else {
        AP_ParamItem *ni;

        ni = new AP_ParamItem;
        *ni = item;

        m_lstAll.push_back(ni);
        m_mapIndex[item.index] = ni;
        m_mapID[item.id] = ni;
    }

    return 0;
}

AP_ParamItem* AP_ParamArray::get(int idx)
{
    AP_ParamItem *pi = NULL;
    std::map<int, AP_ParamItem*>::iterator it;

    it = m_mapIndex.find(idx);
    if( it != m_mapIndex.end() ) {
        pi = it->second;
    }

    return pi;
}

AP_ParamItem* AP_ParamArray::get(std::string id)
{
    AP_ParamItem *pi = NULL;
    std::map<std::string, AP_ParamItem*>::iterator it;

    it = m_mapID.find(id);
    if( it != m_mapID.end() ) {
        pi = it->second;
    }

    return pi;
}

AP_ParamVector* AP_ParamArray::get_allParam(void)
{
    return &m_lstAll;
}

AP_ParamIndexMap* AP_ParamArray::get_paramIndexMap(void)
{
    return &m_mapIndex;
}


int AP_ParamArray::set_paramN(int nParam)
{
    m_nParam = nParam;

    return 0;
}

int AP_ParamArray::get_paramN(void)
{
    int n;

    n = m_nParam;

    return n;
}

int AP_ParamArray::get_loaded(void)
{
    int l;

    l = m_bLoaded;

    return l;
}

int AP_ParamArray::set_loaded(int bl)
{
    m_bLoaded = bl;

    return 0;
}

int AP_ParamArray::set_status(PARAM_RW_STATUS st)
{
    m_stParamReading = st;

    return 0;
}

AP_ParamArray::PARAM_RW_STATUS AP_ParamArray::get_status(int &nParam, int &idxCurr)
{
    AP_ParamArray::PARAM_RW_STATUS st;

    nParam  = m_nParam;
    idxCurr = m_idxCurrent;
    st = m_stParamReading;

    return st;
}

AP_ParamArray::PARAM_RW_STATUS AP_ParamArray::get_status()
{
    AP_ParamArray::PARAM_RW_STATUS st;

    st = m_stParamReading;

    return st;
}

int AP_ParamArray::get_tm_lastRead(uint64_t &t)
{
    t = m_tLastReading;

    return 0;
}

int AP_ParamArray::save(std::string fname)
{
    FILE                        *fp = NULL;
    AP_ParamVector::iterator    it;
    AP_ParamItem                *p;

    // open file
    fp = fopen(fname.c_str(), "wt");
    if( fp == NULL ) {
        printf("Cannot open file: %s", fname.c_str());
        return -1;
    }

    // output waypoints
    fprintf(fp, "#AP parameter number\n");
    fprintf(fp, "%d\n", m_lstAll.size());

    fprintf(fp, "#parameter list\n");
    fprintf(fp, "# index    ID/name    type   value\n");

    for(it=m_lstAll.begin(); it!=m_lstAll.end(); it++) {
        p = *it;

        fprintf(fp, "%4d %17s %2d %f\n",
                p->index,
                p->id,
                (int)(p->type),
                p->value);
    }

    // close file
    fclose(fp);

    return 0;
}

int AP_ParamArray::load(std::string fname)
{
    FILE                        *fp = NULL;

    char                        *buf;
    std::string                 _b;
    int                         max_line_size;
    int                         s, idx, n;
    int                         i1, i2;
    float                       val;

    // open file
    fp = fopen(fname.c_str(), "rt");
    if( fp == NULL ) {
        printf("Cannot open file: %s", fname.c_str());
        return -1;
    }

    // free old contents
    clear();

    // alloc memory buffer
    max_line_size = 1024;
    buf = new char[max_line_size];

    s = 0;
    idx = 0;

    while(!feof(fp)) {
        // read a line
        if( NULL == fgets(buf, max_line_size, fp) )
            break;

        // remove blank & CR
        _b = trim(buf);

        if( _b.size() < 1 )
            continue;

        // skip comment
        if( _b[0] == '#' || _b[0] == ':' )
            continue;

        if( s == 0 ) {
            // read wp number
            sscanf(_b.c_str(), "%d", &n);
            m_nParam = n;
            s = 1;
        } else {
            AP_ParamItem pi;

            // read wp item
            sscanf(_b.c_str(), "%d %s %d %f",
                   &i1, pi.id,
                   &i2, &val);

            pi.index = i1;
            pi.type  = i2;
            pi.value = val;
            pi.modified = 0;

            // insert to map
            set(pi);

            idx ++;
        }
    }

    // set flags
    m_stParamReading = FINISHED;
    m_bLoaded = 1;

    // free buffer
    delete [] buf;

    // close file
    fclose(fp);

    return 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AP_WayPoint::init(void)
{
    idx = -1;

    cmd = 16;

    lat = -9999;
    lng = -9999;
    alt = 50;

    heading = 0;

    param1 = 0;
    param2 = 0;
    param3 = 0;
    param4 = 0;

    current = 0;

    frame = 0;
    autocontinue = 1;
}

void AP_WayPoint::release(void)
{
    return;
}

void AP_WayPoint::set(double lat_, double lng_, double alt_)
{
    lat = lat_;
    lng = lng_;
    alt = alt_;
}

void AP_WayPoint::setPos(double lat_, double lng_)
{
    lat = lat_;
    lng = lng_;
}

void AP_WayPoint::setAlt(double alt_)
{
    alt = alt_;
}

void AP_WayPoint::setHeading(double heading_)
{
    heading = heading_;
}

void AP_WayPoint::get(double &lat_, double &lng_, double &alt_)
{
    lat_ = lat;
    lng_ = lng;
    alt_ = alt;
}

void AP_WayPoint::getPos(double &lat_, double &lng_)
{
    lat_ = lat;
    lng_ = lng;
}

void AP_WayPoint::getAlt(double &alt_)
{
    alt_ = alt;
}

void AP_WayPoint::getHeading(double &heading_)
{
    heading_ = heading;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

AP_WPArray::AP_WPArray()
{
    init();
}

AP_WPArray::~AP_WPArray()
{
    release();
}

void AP_WPArray::init(void)
{
    m_arrWP.clear();

    m_nWP = 0;
    m_iRW = 0;
    m_status = 0;
}

void AP_WPArray::release(void)
{
    if( m_arrWP.size() > 0 ) {
        AP_WayPointMap::iterator it;
        AP_WayPoint *p;

        for(it=m_arrWP.begin(); it!=m_arrWP.end(); it++) {
            p = it->second;
            delete p;
            p = NULL;
        }

        m_arrWP.clear();
    }

    m_nWP = 0;
    m_iRW = 0;
    m_status = 0;
}

int AP_WPArray::set(AP_WPArray *wpa)
{
    // clear old wps
    clear();

    // insert all wps
    AP_WayPointMap::iterator it;
    AP_WayPoint *p;

    for(it=wpa->m_arrWP.begin(); it!=wpa->m_arrWP.end(); it++) {
        p = it->second;

        set(*p);
    }

    return 0;
}

int AP_WPArray::set(AP_WayPoint &wp)
{
    AP_WayPointMap::iterator it;

    it = m_arrWP.find(wp.idx);
    if( it != m_arrWP.end() ) {
        *(it->second) = wp;
    } else {
        AP_WayPoint *p = new AP_WayPoint;
        *p = wp;

        m_arrWP.insert(std::pair<int, AP_WayPoint*>(wp.idx, p));
    }

    return 0;
}

int AP_WPArray::get(AP_WayPoint &wp)
{
    AP_WayPointMap::iterator it;

    it = m_arrWP.find(wp.idx);
    if( it != m_arrWP.end() ) {
        wp = *(it->second);
    } else {
        return -1;
    }

    return 0;
}

AP_WayPoint* AP_WPArray::get(int idx)
{
    AP_WayPointMap::iterator it;
    AP_WayPoint *wp;

    it = m_arrWP.find(idx);
    if( it != m_arrWP.end() ) {
        wp = it->second;
    } else {
        wp = NULL;
    }

    return wp;
}

int AP_WPArray::getAll(AP_WayPointVector &wps)
{
    AP_WayPointMap::iterator it;

    wps.clear();

    for(it=m_arrWP.begin(); it!=m_arrWP.end(); it++) {
        wps.push_back(it->second);
    }

    return 0;
}

AP_WayPointMap* AP_WPArray::getAll(void)
{
    return &m_arrWP;
}

int AP_WPArray::size(void)
{
    return m_arrWP.size();
}

int AP_WPArray::remove(int idxWP)
{
    AP_WayPointMap::iterator it;

    it = m_arrWP.find(idxWP);
    if( it != m_arrWP.end() ) {
        m_arrWP.erase(it);
    } else {
        return -1;
    }

    return 0;
}

int AP_WPArray::clear(void)
{
    release();

    return 0;
}

int AP_WPArray::save(std::string fname)
{
    FILE                        *fp = NULL;
    AP_WayPointMap::iterator    it;
    AP_WayPoint                 *p;

    // open file
    fp = fopen(fname.c_str(), "wt");
    if( fp == NULL ) {
        printf("Cannot open file: %s", fname.c_str());
        return -1;
    }

    // output waypoints
    fprintf(fp, "#waypoint number\n");
    fprintf(fp, "%d\n", m_arrWP.size());

    fprintf(fp, "#waypoints list\n");
    fprintf(fp, "# idx              lat                   lng                  alt         heading\n");

    for(it=m_arrWP.begin(); it!=m_arrWP.end(); it++) {
        p = it->second;

        fprintf(fp, "%4d %24.16f %24.16f %12.3f %12.3f\n",
                p->idx,
                p->lat, p->lng, p->alt,
                p->heading);
    }

    // close file
    fclose(fp);

    return 0;
}

int AP_WPArray::load(std::string fname)
{
    FILE                        *fp = NULL;

    char                        *buf;
    std::string                 _b;
    int                         max_line_size;
    int                         s, idx, n;
    int                         i1;
    double                      lat, lng, alt, heading;

    // open file
    fp = fopen(fname.c_str(), "rt");
    if( fp == NULL ) {
        printf("Cannot open file: %s", fname.c_str());
        return -1;
    }

    // free old contents
    clear();

    // alloc memory buffer
    max_line_size = 1024;
    buf = new char[max_line_size];

    s = 0;
    idx = 0;

    while(!feof(fp)) {
        // read a line
        if( NULL == fgets(buf, max_line_size, fp) )
            break;

        // remove blank & CR
        _b = trim(buf);

        if( _b.size() < 1 )
            continue;

        // skip comment
        if( _b[0] == '#' || _b[0] == ':' )
            continue;

        if( s == 0 ) {
            // read wp number
            sscanf(_b.c_str(), "%d", &n);
            s = 1;
        } else {
            // read wp item
            sscanf(_b.c_str(), "%d %lf %lf %lf %lf",
                   &i1,
                   &lat, &lng, &alt, &heading);

            AP_WayPoint w;

            w.idx = i1;
            w.lat = lat;
            w.lng = lng;
            w.alt = alt;
            w.heading = heading;

            // insert to map
            set(w);

            idx ++;
        }
    }

    // free buffer
    delete [] buf;

    // close file
    fclose(fp);

    return 0;
}
