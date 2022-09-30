#pragma once

#include "global.h"
#include "qdebug.h"
#include "storebase.h"

class OFString;
class OFCondition;
struct T_ASC_Parameters;
struct T_ASC_Association;
struct T_DIMSE_StoreProgress;
struct T_DIMSE_C_StoreRQ;

class DICOMBASE_EXPORT StoreDcmtk : public StoreBase
{
    Q_OBJECT

public:
    StoreDcmtk(QObject *parent = nullptr);
    ~StoreDcmtk();

    QDebug &qDebugDelay();
    void resetQDebugDelay();

protected:
    void store() override;

private:
    OFCondition CheckSOPInstance(const OFString &sopClassUID, const OFString &sopInstanceUID,
                                 const OFString &transferSyntaxUID, const bool checkValues);
    OFCondition AddPresentationContext(T_ASC_Parameters *params);
    OFCondition storeSCU(T_ASC_Association *assoc, const char *fname);
    static void progressCallback(void *parent, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *req);

    QDebug m_qDebugDelay;
};

inline QDebug &StoreDcmtk::qDebugDelay()
{
    return m_qDebugDelay;
}

inline void StoreDcmtk::resetQDebugDelay()
{
    m_qDebugDelay = QDebug(QtDebugMsg);
}
