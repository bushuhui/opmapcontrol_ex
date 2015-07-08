#ifndef __UAS_TYPES_H__
#define __UAS_TYPES_H__


#include <stdio.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <deque>


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct AP_ParamItem
{
public:
    int             index;                      ///< index number
    char            id[17];                     ///< ID name
    int             type;                       ///< value type
    float           value;                      ///< value

    int             modified;                   ///< modified flag

public:
    int8_t      toInt8(void);
    uint8_t     toUint8(void);
    int16_t     toInt16(void);
    uint16_t    toUint16(void);
    int32_t     toInt32(void);
    uint32_t    toUint32(void);
    float       toFloat(void);

    void        fromInt8(int8_t v);
    void        fromUint8(uint8_t v);
    void        fromInt16(int16_t v);
    void        fromUint16(uint16_t v);
    void        fromInt32(int32_t v);
    void        fromUint32(uint32_t v);
    void        fromFloat(float v);

public:
    AP_ParamItem();
    virtual ~AP_ParamItem();

    void init();
    void release();
};

typedef std::map<int, AP_ParamItem*>            AP_ParamIndexMap;
typedef std::map<std::string, AP_ParamItem*>    AP_ParamIDMap;
typedef std::vector<AP_ParamItem*>              AP_ParamVector;


class AP_ParamArray
{
public:
    enum PARAM_RW_STATUS {
        IDLE,
        READING,
        WRITING,
        FINISHED,
        FAILED
    };

public:
    AP_ParamArray();
    ~AP_ParamArray();

    void init(void);
    void release(void);

    int size(void);
    int reserve(int n);
    int clear(void);

    int lock(void);
    int unlock(void);

    int set(AP_ParamArray &pa);
    int set(AP_ParamItem &item);
    AP_ParamItem* get(int idx);
    AP_ParamItem* get(std::string id);

    AP_ParamVector* get_allParam(void);
    AP_ParamIndexMap* get_paramIndexMap(void);

    int set_paramN(int nParam);
    int get_paramN(void);

    int get_loaded(void);
    int set_loaded(int bl);

    int set_status(PARAM_RW_STATUS st);
    PARAM_RW_STATUS get_status(int &nParam, int &idxCurr);
    PARAM_RW_STATUS get_status(void);

    int get_tm_lastRead(uint64_t &t);

    int save(std::string fname);
    int load(std::string fname);

protected:
    AP_ParamVector              m_lstAll;
    AP_ParamIndexMap            m_mapIndex;
    AP_ParamIDMap               m_mapID;

    int                         m_nParam;               // paramter number
    int                         m_idxCurrent;           // current reading index

    PARAM_RW_STATUS             m_stParamReading;       // parameter reading status
                                                        //  0 - no reading
                                                        //  1 - reading
                                                        //  2 - read finished
                                                        //  3 - failed
    uint64_t                    m_tLastReading;         // last read time

    int                         m_bLoaded;              // loaded
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class AP_WayPoint
{
public:
    AP_WayPoint() { init(); }
    ~AP_WayPoint() { release(); }

    void init(void);
    void release(void);

    void set(double lat_, double lng_, double alt_);
    void setPos(double lat_, double lng_);
    void setAlt(double alt_);
    void setHeading(double heading_);

    void get(double &lat_, double &lng_, double &alt_);
    void getPos(double &lat_, double &lng_);
    void getAlt(double &alt_);
    void getHeading(double &heading_);

public:
    int         idx;                    ///< index number

    int         cmd;                    ///< mission command

    double      lat, lng;               ///< latitdue & longtitude
    double      alt;                    ///< altitude
    double      heading;                ///< heading (0:North, 90: Est, 180: South, 270: West)

    float       param1;                 ///< PARAM1, see MAV_CMD enum
    float       param2;                 ///< PARAM2, see MAV_CMD enum
    float       param3;                 ///< PARAM3, see MAV_CMD enum
    float       param4;                 ///< PARAM4, see MAV_CMD enum

    int         current;                ///< current mission (1:True, 0:False)

    int         frame;                  ///< The coordinate system of the MISSION. see MAV_FRAME in mavlink_types.h
    uint8_t     autocontinue;           ///< autocontinue to next wp
};


typedef std::map<int, AP_WayPoint*>     AP_WayPointMap;
typedef std::vector<AP_WayPoint*>       AP_WayPointVector;


class AP_WPArray
{
public:
    AP_WPArray();
    ~AP_WPArray();

    void init(void);
    void release(void);

    int set(AP_WPArray *wpa);
    int set(AP_WayPoint &wp);
    int get(AP_WayPoint &wp);
    AP_WayPoint* get(int idx);
    int getAll(AP_WayPointVector &wps);
    AP_WayPointMap* getAll();

    int size(void);

    int remove(int idxWP);
    int clear(void);

    int save(std::string fname);
    int load(std::string fname);

protected:
    AP_WayPointMap      m_arrWP;        ///< waypoints map

public:
    int                 m_nWP;          ///< total waypoints
    int                 m_iRW;          ///< current waypoint
    int                 m_status;       ///< R/W status
    uint64_t            m_tLast;        ///< Last R/W timestamp (in ms)
};

#endif // end of __UAS_TYPES_H__
