#include "analytics.h"

Analytics::Analytics() {
    trackerId = "UA-118159593-1";
    url = "http://www.google-analytics.com/collect";
    header = "application/x-www-form-urlencoded";
}

void Analytics::post()
{
    // Get hostname so we can set it as a parameter.
    QString hostname = QHostInfo::localHostName() + "." + QHostInfo::localDomainName();

    // Generate a unique ID to use as the Client ID.
    QUuid uuid = QUuid::createUuid();

    // Create an http network request.
    QNetworkRequest req(QUrl("http://www.google-analytics.com/collect"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    // Build up the query parameters.
    QUrlQuery query;
    query.addQueryItem("v", "0.4"); // Version
    query.addQueryItem("tid", "UA-XXXXXXXX-1"); // Tracking ID - use value assigned to you by Google Analytics
    query.addQueryItem("cid", uuid.toString()); // Client ID

    if (ui->hitTypeComboBox->currentText() == "pageview") {
        query.addQueryItem("t", "pageview"); // Page view hit type
        query.addQueryItem("dh", hostname); // Document hostname
        query.addQueryItem("dp", ui->pageViewPageLineEdit->text()); // Page
        query.addQueryItem("dt", ui->pageViewTitleLineEdit->text()); // Title
    }

    if (ui->hitTypeComboBox->currentText() == "event") {
        query.addQueryItem("t", "event"); // Event hit type
        query.addQueryItem("ec", ui->eventCategoryLinedit->text()); // Event category
        query.addQueryItem("ea", ui->eventActionLineEdit->text()); // Event action
        query.addQueryItem("el", ui->eventLabelLineEdit->text()); // Event label
        query.addQueryItem("ev", ui->eventValueLineEdit->text()); // Event value
    }

    QByteArray data;
    data.append(query.query());
    qDebug() << data; // Output for debug purposes.
    nam->post(req, data);
}