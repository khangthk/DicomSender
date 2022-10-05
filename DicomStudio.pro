TEMPLATE = subdirs

SUBDIRS += \
    DicomBase \
    DicomSender \
    DicomTags \
    DicomViewer

DicomSender.depends = DicomBase
DicomTags.depends = DicomBase
DicomViewer.depends = DicomBase
