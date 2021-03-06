#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QDeclarativeView>

class QSettings;
class AccountsListModel;
class AccountsFoldersModel;
class EasyTweakModel;
class SettingsListModel;
class FoldersModel;

class MainView : public QObject
{
    Q_OBJECT
public:
    explicit MainView(QDeclarativeView* view,QObject *parent = 0);

    QString getSourcePath() const;

    void showFullScreen();

signals:

public slots:

private:
    QDeclarativeView* mView;
    AccountsListModel* mAccountsModel;
    EasyTweakModel* mEasyTweakModel;
    SettingsListModel* mSettingsModel;
    FoldersModel* mFoldersModel;
    AccountsFoldersModel* mAccountsFoldersModel;
};

#endif // MAINVIEW_H
