#include "dcmtkhelper.h"

#include <dcmtk/dcmdata/dcfilefo.h>

bool DcmtkHelper::isDicom(const QString &file)
{
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(file.toStdWString().c_str(), EXS_Unknown, EGL_noChange, DCM_MaxReadLength, ERM_autoDetect);
    return status.good();
}
