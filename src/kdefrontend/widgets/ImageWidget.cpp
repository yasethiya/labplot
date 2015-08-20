
/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "ImageWidget.h"
#include "backend/worksheet/CustomItem.h"
#include "kdefrontend/widgets/CustomItemWidget.h"
#include "commonfrontend/widgets/qxtspanslider.h"
#include "commonfrontend/widgets/TreeViewComboBox.h"
#include "backend/core/AspectTreeModel.h"
#include "backend/core/PlotCurve.h"

#include <QGridLayout>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <KUrlCompletion>
#include <QFileDialog>
#include <QDir>


ImageWidget::ImageWidget(QWidget *parent): QWidget(parent), m_aspectTreeModel(0) {
    ui.setupUi(this);

    QHBoxLayout* hboxLayout = new QHBoxLayout(ui.tSymbol);
    customItemWidget = new CustomItemWidget(ui.tSymbol);
    hboxLayout->addWidget(customItemWidget);
    customItemWidget->hidePositionWidgets();

    ui.kleFileName->setClearButtonShown(true);
    ui.bOpen->setIcon( KIcon("document-open") );

    KUrlCompletion *comp = new KUrlCompletion();
    ui.kleFileName->setCompletionObject(comp);

    QGridLayout* generalTabLayout = static_cast<QGridLayout*>(ui.tGeneral->layout());
    cbActiveCurve = new TreeViewComboBox(ui.tGeneral);
    generalTabLayout->addWidget(cbActiveCurve, 4, 2, 1, 6);

    QGridLayout* editTabLayout = static_cast<QGridLayout*>(ui.tEdit->layout());
    editTabLayout->setContentsMargins(2,2,2,2);
    editTabLayout->setHorizontalSpacing(2);
    editTabLayout->setVerticalSpacing(4);

    ssIntensity = new QxtSpanSlider(Qt::Horizontal,ui.tEdit);
    ssIntensity->setRange(0, 100);
    editTabLayout->addWidget(ssIntensity, 1, 1);

    ssForeground = new QxtSpanSlider(Qt::Horizontal,ui.tEdit);
    ssForeground->setRange(0, 100);
    editTabLayout->addWidget(ssForeground, 2, 1);

    ssHue = new QxtSpanSlider(Qt::Horizontal,ui.tEdit);
    ssHue->setRange(0, 360);
    editTabLayout->addWidget(ssHue, 3, 1);

    ssSaturation = new QxtSpanSlider(Qt::Horizontal,ui.tEdit);
    ssSaturation->setRange(0,100);
    editTabLayout->addWidget(ssSaturation, 4, 1);

    ssValue = new QxtSpanSlider(Qt::Horizontal,ui.tEdit);
    ssValue->setRange(0,100);
    editTabLayout->addWidget(ssValue, 5, 1);

    ui.cbGraphType->addItem(i18n("Cartesian (x, y)"));
    ui.cbGraphType->addItem(i18n("Polar (x, y°)"));
    ui.cbGraphType->addItem(i18n("Polar (x, y(rad))"));
    ui.cbGraphType->addItem(i18n("Logarithmic (ln(x), y)"));
    ui.cbGraphType->addItem(i18n("Logarithmic (x, ln(y))"));

    ui.cbPlotImageType->addItem(i18n("Original Image"));
    ui.cbPlotImageType->addItem(i18n("Processed Image"));

    ui.cbXErrorType->addItem(i18n("No Error"));
    ui.cbXErrorType->addItem(i18n("symmetric"));
    ui.cbXErrorType->addItem(i18n("asymmetric"));

    ui.cbYErrorType->addItem(i18n("No Error"));
    ui.cbYErrorType->addItem(i18n("symmetric"));
    ui.cbYErrorType->addItem(i18n("asymmetric"));

    //SLOTS
    // geometry
    connect( ui.sbRotation, SIGNAL(valueChanged(double)), this, SLOT(rotationChanged(double)) );
    connect( ui.bOpen, SIGNAL(clicked(bool)), this, SLOT(selectFile()));
    connect( ui.kleFileName, SIGNAL(returnPressed()), this, SLOT(fileNameChanged()) );
    connect( ui.kleFileName, SIGNAL(clearButtonClicked()), this, SLOT(fileNameChanged()) );
    connect( ui.cbPlotImageType, SIGNAL(currentIndexChanged(int)), this, SLOT(plotImageTypeChanged(int)) );
    connect( ui.cbXErrorType, SIGNAL(currentIndexChanged(int)), this, SLOT(xErrorTypeChanged(int)) );
    connect( ui.cbYErrorType, SIGNAL(currentIndexChanged(int)), this, SLOT(yErrorTypeChanged(int)) );
    connect( ssIntensity, SIGNAL(spanChanged(int,int)), this, SLOT(intensitySpanChanged(int,int)) );
    connect( ssForeground, SIGNAL(spanChanged(int,int)), this, SLOT(foregroundSpanChanged(int,int)) );
    connect( ssHue, SIGNAL(spanChanged(int,int)), this, SLOT(hueSpanChanged(int,int)) );
    connect( ssSaturation, SIGNAL(spanChanged(int,int)), this, SLOT(saturationSpanChanged(int,int)) );
    connect( ssValue, SIGNAL(spanChanged(int,int)), this, SLOT(valueSpanChanged(int,int)) );
    connect( ui.rbIntensity, SIGNAL(clicked()), this, SLOT(rbClicked()) );
    connect( ui.rbForeground, SIGNAL(clicked()), this, SLOT(rbClicked()) );
    connect( ui.rbHue, SIGNAL(clicked()), this, SLOT(rbClicked()) );
    connect( ui.rbSaturation, SIGNAL(clicked()), this, SLOT(rbClicked()) );
    connect( ui.rbValue, SIGNAL(clicked()), this, SLOT(rbClicked()) );
    connect( ui.sbMinSegmentLength, SIGNAL(valueChanged(int)), this, SLOT(minSegmentLengthChanged(int)) );
    connect( ui.sbPointSeparation, SIGNAL(valueChanged(int)), this, SLOT(pointSeparationChanged(int)) );
    connect( cbActiveCurve, SIGNAL(currentModelIndexChanged(QModelIndex)), this, SLOT(activeCurveChanged(QModelIndex)) );

    //axis point
    connect( ui.cbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(graphTypeChanged()) );
    connect( ui.sbPoisitionX1, SIGNAL(valueChanged(double)), this, SLOT(logicalPositionChanged()) );
    connect( ui.sbPoisitionY1, SIGNAL(valueChanged(double)), this, SLOT(logicalPositionChanged()) );
    connect( ui.sbPoisitionX2, SIGNAL(valueChanged(double)), this, SLOT(logicalPositionChanged()) );
    connect( ui.sbPoisitionY2, SIGNAL(valueChanged(double)), this, SLOT(logicalPositionChanged()) );
    connect( ui.sbPoisitionX3, SIGNAL(valueChanged(double)), this, SLOT(logicalPositionChanged()) );
    connect( ui.sbPoisitionY3, SIGNAL(valueChanged(double)), this, SLOT(logicalPositionChanged()) );
}

ImageWidget::~ImageWidget() {
    if (m_aspectTreeModel)
        delete m_aspectTreeModel;
}

void ImageWidget::setImages(QList<Image*> list) {
    m_imagesList = list;
    m_image = list.first();
    Q_ASSERT(m_image);
    m_aspectTreeModel = new AspectTreeModel(m_image->parentAspect());
    setModel();
    this->load();
    initConnections();
    handleWidgetActions();
}

void ImageWidget::initConnections() {
    connect( m_image, SIGNAL(fileNameChanged(QString)), this, SLOT(imageFileNameChanged(QString)) );
    connect( m_image, SIGNAL(rotationAngleChanged(float)), this, SLOT(imageRotationAngleChanged(float)) );
    connect( m_image, SIGNAL(aspectRemoved(const AbstractAspect*,const AbstractAspect*,const AbstractAspect*)),
             this,SLOT(updateCustomItemList()) );
    connect( m_image, SIGNAL(aspectAdded(const AbstractAspect*)), this,SLOT(updateCustomItemList()) );
    connect( m_image, SIGNAL(axisPointsChanged(Image::ReferencePoints)), this, SLOT(imageAxisPointsChanged(Image::ReferencePoints)) );
    connect( m_image, SIGNAL(settingsChanged(Image::EditorSettings)), this, SLOT(imageEditorSettingsChanged(Image::EditorSettings)) );
    connect( m_image, SIGNAL(minSegmentLengthChanged(int)), this, SLOT(imageMinSegmentLengthChanged(int)) );
    connect( m_image, SIGNAL(activeCurveChanged(const PlotCurve*)), this, SLOT(updateCustomItemList()) );
    if (m_image->activeCurve())
        connect( m_image->activeCurve(), SIGNAL(aspectAdded(const AbstractAspect*)), this, SLOT(updateCustomItemList()) );
}


void ImageWidget::setModel() {
    QList<const char*>  list;
    list<<"Datapicker"<<"Image"<<"PlotCurve";
    cbActiveCurve->setTopLevelClasses(list);

    list.clear();
    list<<"PlotCurve";
    m_aspectTreeModel->setSelectableAspects(list);
    cbActiveCurve->setSelectableClasses(list);
    cbActiveCurve->setModel(m_aspectTreeModel);
}


void ImageWidget::handleWidgetActions() {
    QString fileName =  ui.kleFileName->text().trimmed();
    if (!fileName.isEmpty()) {
        ui.tEdit->setEnabled(true);
        ui.cbGraphType->setEnabled(true);
        ui.cbXErrorType->setEnabled(true);
        ui.cbYErrorType->setEnabled(true);
        ui.sbPoisitionX1->setEnabled(true);
        ui.sbPoisitionX2->setEnabled(true);
        ui.sbPoisitionX3->setEnabled(true);
        ui.sbPoisitionY1->setEnabled(true);
        ui.sbPoisitionY2->setEnabled(true);
        ui.sbPoisitionY3->setEnabled(true);
    } else {
        ui.tEdit->setEnabled(false);
        ui.cbGraphType->setEnabled(false);
        ui.cbXErrorType->setEnabled(false);
        ui.cbYErrorType->setEnabled(false);
        ui.sbPoisitionX1->setEnabled(false);
        ui.sbPoisitionX2->setEnabled(false);
        ui.sbPoisitionX3->setEnabled(false);
        ui.sbPoisitionY1->setEnabled(false);
        ui.sbPoisitionY2->setEnabled(false);
        ui.sbPoisitionY3->setEnabled(false);
    }

    updateCustomItemList();
}

//**********************************************************
//****** SLOTs for changes triggered in ImageWidget ********
//**********************************************************
void ImageWidget::selectFile() {
    KConfigGroup conf(KSharedConfig::openConfig(), "ImageWidget");
    QString dir = conf.readEntry("LastImageDir", "");
    QString path = QFileDialog::getOpenFileName(this, i18n("Select the image file"), dir);
    if (path.isEmpty())
        return; //cancel was clicked in the file-dialog

    int pos = path.lastIndexOf(QDir::separator());
    if (pos!=-1) {
        QString newDir = path.left(pos);
        if (newDir!=dir)
            conf.writeEntry("LastImageDir", newDir);
    }

    ui.kleFileName->setText( path );
    handleWidgetActions();

    foreach(Image* image, m_imagesList)
        image->setFileName(path);
}

void ImageWidget::fileNameChanged(){
    if (m_initializing)
        return;

    handleWidgetActions();

    QString fileName = ui.kleFileName->text();
    foreach(Image* image, m_imagesList){
        image->setFileName(fileName);
    }
}

void ImageWidget::graphTypeChanged() {
    if (m_initializing)
        return;

    Image::ReferencePoints points = m_image->axisPoints();
    points.type = Image::GraphType(ui.cbGraphType->currentIndex());

    foreach(Image* image, m_imagesList)
        image->setAxisPoints(points);
}

void ImageWidget::logicalPositionChanged() {
    if (m_initializing)
        return;

    Image::ReferencePoints points = m_image->axisPoints();
    points.logicalPos[0].setX(ui.sbPoisitionX1->value());
    points.logicalPos[0].setY(ui.sbPoisitionY1->value());
    points.logicalPos[1].setX(ui.sbPoisitionX2->value());
    points.logicalPos[1].setY(ui.sbPoisitionY2->value());
    points.logicalPos[2].setX(ui.sbPoisitionX3->value());
    points.logicalPos[2].setY(ui.sbPoisitionY3->value());

    foreach(Image* image, m_imagesList)
        image->setAxisPoints(points);
}

void ImageWidget::activeCurveChanged(const QModelIndex& index) {
    if (m_initializing)
        return;

    AbstractAspect* aspect = static_cast<AbstractAspect*>(index.internalPointer());
    PlotCurve* curve = dynamic_cast<PlotCurve*>(aspect);

    foreach(Image* image, m_imagesList)
        image->setActiveCurve(curve);
}

void ImageWidget::rotationChanged(double value){
    if (m_initializing)
        return;

    foreach(Image* image, m_imagesList)
        image->setRotationAngle(value);
}


void ImageWidget::intensitySpanChanged(int lowerLimit, int upperLimit) {
    if (m_initializing)
        return;

    m_initializing = true;
    ui.rbIntensity->setChecked(true);
    m_initializing = false;

    Image::EditorSettings settings = m_image->settings();
    settings.type = Image::Intensity;
    settings.intensityThresholdHigh = upperLimit;
    settings.intensityThresholdLow = lowerLimit;
    foreach(Image* image, m_imagesList)
        image->setSettings(settings);
}

void ImageWidget::foregroundSpanChanged(int lowerLimit, int upperLimit) {
    if (m_initializing)
        return;

    m_initializing = true;
    ui.rbForeground->setChecked(true);
    m_initializing = false;

    Image::EditorSettings settings = m_image->settings();
    settings.type = Image::Foreground;
    settings.foregroundThresholdHigh = upperLimit;
    settings.foregroundThresholdLow = lowerLimit;
    foreach(Image* image, m_imagesList)
        image->setSettings(settings);
}

void ImageWidget::hueSpanChanged(int lowerLimit, int upperLimit) {
    if (m_initializing)
        return;

    m_initializing = true;
    ui.rbHue->setChecked(true);
    m_initializing = false;

    Image::EditorSettings settings = m_image->settings();
    settings.type = Image::Hue;
    settings.hueThresholdHigh = upperLimit;
    settings.hueThresholdLow = lowerLimit;
    foreach(Image* image, m_imagesList)
        image->setSettings(settings);
}

void ImageWidget::saturationSpanChanged(int lowerLimit, int upperLimit) {
    if (m_initializing)
        return;

    m_initializing = true;
    ui.rbSaturation->setChecked(true);
    m_initializing = false;

    Image::EditorSettings settings = m_image->settings();
    settings.type = Image::Saturation;
    settings.saturationThresholdHigh = upperLimit;
    settings.saturationThresholdLow = lowerLimit;
    foreach(Image* image, m_imagesList)
        image->setSettings(settings);
}

void ImageWidget::valueSpanChanged(int lowerLimit, int upperLimit) {
    if (m_initializing)
        return;

    m_initializing = true;
    ui.rbValue->setChecked(true);
    m_initializing = false;

    Image::EditorSettings settings = m_image->settings();
    settings.type = Image::Value;
    settings.valueThresholdHigh = upperLimit;
    settings.valueThresholdLow = lowerLimit;
    foreach(Image* image, m_imagesList)
        image->setSettings(settings);
}

void ImageWidget::rbClicked() {
    if (m_initializing)
        return;

    Image::EditorSettings settings = m_image->settings();
    if (ui.rbIntensity->isChecked()) {
        settings.type = Image::Intensity;
    } else if (ui.rbForeground->isChecked()) {
        settings.type = Image::Foreground;
    } else if (ui.rbHue->isChecked()) {
        settings.type = Image::Hue;
    } else if (ui.rbSaturation->isChecked()) {
        settings.type = Image::Saturation;
    } else {
        settings.type = Image::Value;
    }

    foreach(Image* image, m_imagesList)
        image->setSettings(settings);
}

void ImageWidget::plotImageTypeChanged(int index) {
    if (m_initializing)
        return;

    foreach(Image* image, m_imagesList)
        image->setPlotImageType(Image::PlotImageType(index));
}

void ImageWidget::xErrorTypeChanged(int index) {
    if (m_initializing)
        return;
    Image::Errors errors = m_image->plotErrors();
    errors.x = Image::ErrorType(index);

    foreach(Image* image, m_imagesList)
        image->setPlotErrors(errors);
}

void ImageWidget::yErrorTypeChanged(int index) {
    if (m_initializing)
        return;

    Image::Errors errors = m_image->plotErrors();
    errors.y = Image::ErrorType(index);

    foreach(Image* image, m_imagesList)
        image->setPlotErrors(errors);
}

void ImageWidget::minSegmentLengthChanged(int value) {
    if (m_initializing)
        return;

    foreach(Image* image, m_imagesList)
        image->setminSegmentLength(value);
}

void ImageWidget::pointSeparationChanged(int value) {
    if (m_initializing)
        return;

    foreach(Image* image, m_imagesList)
        image->setPointSeparation(value);
}

void ImageWidget::setModelIndexFromCurve(TreeViewComboBox* cb, const PlotCurve* curve){
    if (curve)
        cb->setCurrentModelIndex(m_aspectTreeModel->modelIndexOfAspect(curve));
    else
        cb->setCurrentModelIndex(QModelIndex());
}
//*********************************************************
//****** SLOTs for changes triggered in Image *********
//*********************************************************
void ImageWidget::imageFileNameChanged(const QString& name) {
    m_initializing = true;
    ui.kleFileName->setText(name);
    m_initializing = false;
}

void ImageWidget::imageRotationAngleChanged(float angle){
    m_initializing = true;
    ui.sbRotation->setValue(angle);
    m_initializing = false;
}

void ImageWidget::imageAxisPointsChanged(const Image::ReferencePoints& axisPoints) {
    m_initializing = true;
    ui.cbGraphType->setCurrentIndex((int) axisPoints.type);
    ui.sbPoisitionX1->setValue(axisPoints.logicalPos[0].x());
    ui.sbPoisitionY1->setValue(axisPoints.logicalPos[0].y());
    ui.sbPoisitionX2->setValue(axisPoints.logicalPos[1].x());
    ui.sbPoisitionY2->setValue(axisPoints.logicalPos[1].y());
    ui.sbPoisitionX3->setValue(axisPoints.logicalPos[2].x());
    ui.sbPoisitionY3->setValue(axisPoints.logicalPos[2].y());
    m_initializing = false;
}

void ImageWidget::imageEditorSettingsChanged(const Image::EditorSettings& settings) {
    m_initializing = true;
    ssIntensity->setSpan(settings.intensityThresholdLow, settings.intensityThresholdHigh);
    ssForeground->setSpan(m_image->settings().foregroundThresholdLow, settings.foregroundThresholdHigh);
    ssHue->setSpan(settings.hueThresholdLow, settings.hueThresholdHigh);
    ssSaturation->setSpan(settings.saturationThresholdLow, settings.saturationThresholdHigh);
    ssValue->setSpan(settings.valueThresholdLow, settings.valueThresholdHigh);
    ui.rbIntensity->setChecked(settings.type == Image::Intensity);
    ui.rbForeground->setChecked(settings.type == Image::Foreground);
    ui.rbHue->setChecked(settings.type == Image::Hue);
    ui.rbSaturation->setChecked(settings.type == Image::Saturation);
    ui.rbValue->setChecked(settings.type == Image::Value);
    m_initializing = false;
}

void ImageWidget::imageMinSegmentLengthChanged(const int value) {
    m_initializing = true;
    ui.sbMinSegmentLength->setValue(value);
    m_initializing = false;
}

void ImageWidget::plotErrorsChanged(Image::Errors errors){
    m_initializing = true;
    ui.cbXErrorType->setCurrentIndex((int) errors.x);
    ui.cbYErrorType->setCurrentIndex((int) errors.y);
    m_initializing = false;
}

void ImageWidget::updateCustomItemList() {
    QList<CustomItem*> itemsList;
    if (m_image->activeCurve())
        itemsList = m_image->activeCurve()->children<CustomItem>(AbstractAspect::IncludeHidden);
    else
        itemsList = m_image->children<CustomItem>(AbstractAspect::IncludeHidden);

    customItemWidget->setCustomItems(itemsList);
}
//**********************************************************
//******************** SETTINGS ****************************
//**********************************************************
void ImageWidget::load() {
    if(m_image == NULL)
        return;

    m_initializing = true;
    ui.kleFileName->setText( m_image->fileName() );
    ui.cbGraphType->setCurrentIndex((int) m_image->axisPoints().type);
    ui.sbPoisitionX1->setValue(m_image->axisPoints().logicalPos[0].x());
    ui.sbPoisitionY1->setValue(m_image->axisPoints().logicalPos[0].y());
    ui.sbPoisitionX2->setValue(m_image->axisPoints().logicalPos[1].x());
    ui.sbPoisitionY2->setValue(m_image->axisPoints().logicalPos[1].y());
    ui.sbPoisitionX3->setValue(m_image->axisPoints().logicalPos[2].x());
    ui.sbPoisitionY3->setValue(m_image->axisPoints().logicalPos[2].y());
    ui.cbPlotImageType->setCurrentIndex((int) m_image->plotImageType);
    ui.cbXErrorType->setCurrentIndex((int) m_image->plotErrors().x);
    ui.cbYErrorType->setCurrentIndex((int) m_image->plotErrors().y);
    ssIntensity->setSpan(m_image->settings().intensityThresholdLow, m_image->settings().intensityThresholdHigh);
    ssForeground->setSpan(m_image->settings().foregroundThresholdLow, m_image->settings().foregroundThresholdHigh);
    ssHue->setSpan(m_image->settings().hueThresholdLow, m_image->settings().hueThresholdHigh);
    ssSaturation->setSpan(m_image->settings().saturationThresholdLow, m_image->settings().saturationThresholdHigh);
    ssValue->setSpan(m_image->settings().valueThresholdLow, m_image->settings().valueThresholdHigh);
    ui.rbIntensity->setChecked(m_image->settings().type == Image::Intensity);
    ui.rbForeground->setChecked(m_image->settings().type == Image::Foreground);
    ui.rbHue->setChecked(m_image->settings().type == Image::Hue);
    ui.rbSaturation->setChecked(m_image->settings().type == Image::Saturation);
    ui.rbValue->setChecked(m_image->settings().type == Image::Value);
    ui.sbPointSeparation->setValue(m_image->pointSeparation());
    ui.sbMinSegmentLength->setValue(m_image->minSegmentLength());
    this->setModelIndexFromCurve(cbActiveCurve, m_image->activeCurve());
    m_initializing = false;
}
