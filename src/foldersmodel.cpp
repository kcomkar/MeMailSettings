#include "foldersmodel.h"

#include <QMailAccount>
#include <QMailStore>

FolderItem::FolderItem(const QMailFolder::StandardFolder &standardFolder, const QMailFolder &mailFolder, QObject *parent)
    : QObject(parent),
      mStandardFolder(standardFolder),
      mFolder(mailFolder)
{
    initItem();
}

QMailFolderId FolderItem::getFolderId() const
{
    return mFolder.id();
}

void FolderItem::initItem()
{
    switch(mStandardFolder)
    {
    case QMailFolder::InboxFolder:
    {
        mStandardFolderName = QString("Inbox");
        mIconForFolder = QString("icon-directory-incoming");
        break;
    }
    case QMailFolder::DraftsFolder:
    {
        mStandardFolderName = QString("Drafts");
        mIconForFolder = QString("icon-directory-draft");
        break;
    }
    case QMailFolder::TrashFolder:
    {
        mStandardFolderName = QString("Trash");
        mIconForFolder = QString("icon-directory-trash");
        break;
    }
    case QMailFolder::SentFolder:
    {
        mStandardFolderName = QString("Sent Items");
        mIconForFolder = QString("icon-directory-send");
        break;
    }
    case QMailFolder::JunkFolder:
    {
        mStandardFolderName = QString("Junk");
        mIconForFolder = QString("icon-directory-spam");
        break;
    }
    default:
        break;
    }
}

QString FolderItem::getDisplayName() const
{
    if (!mFolder.displayName().isEmpty())
        return mFolder.displayName();
    return qtTrId("Local Storage");
}

//----------------------------------------------------------------------------------------------------------
// class FoldersModel

FoldersModel::FoldersModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    mStandardFoldersEnum.append(QMailFolder::InboxFolder);
    mStandardFoldersEnum.append(QMailFolder::DraftsFolder);
    mStandardFoldersEnum.append(QMailFolder::SentFolder);
    mStandardFoldersEnum.append(QMailFolder::TrashFolder);
    mStandardFoldersEnum.append(QMailFolder::JunkFolder);

    QHash<int, QByteArray> roles;
    roles[standardNameRole] = "standardNameRole";
    roles[bindedFolderNameRole] = "bindedFolderNameRole";
    roles[standardFolderRole] = "standardFolderRole";
    roles[iconForStandartFolderRole] = "iconForStandartFolderRole";
    setRoleNames(roles);
}

void FoldersModel::initModel()
{
    connect (QMailStore::instance(), SIGNAL(accountsUpdated(const QMailAccountIdList&)),
                      this, SLOT (onAccountsUpdated(const QMailAccountIdList&)));

    mStandardFolders.clear();
    QMailAccountId accountId(mAccountId);
    if (accountId.isValid())
    {
        QMailAccount account(accountId);
        foreach (const QMailFolder::StandardFolder& stFolder, mStandardFoldersEnum)
        {
            const QMailFolderId& standardFolderId = account.standardFolder(stFolder);
            QMailFolder folder(standardFolderId);
            FolderItem* item = new FolderItem(stFolder, folder, this);
            mStandardFolders.insert(stFolder, item);
        }
    }

    reset();
}

void FoldersModel::onAccountsUpdated(const QMailAccountIdList & ids)
{
    QMailAccountId accountId(mAccountId);

    if (!accountId.isValid())
    {
        return;
    }

    if (ids.contains(accountId))
    {
        QMailAccount account(accountId);
        QList<QModelIndex> changedIndexes;
        foreach (const QMailFolder::StandardFolder& stFolder, mStandardFoldersEnum)
        {
            const QMailFolderId& standardFolderId = account.standardFolder(stFolder);

            if (standardFolderId != mStandardFolders.value(stFolder)->getFolderId())
            {
                QMailFolder folder(standardFolderId);
                FolderItem* item = new FolderItem(stFolder, folder, this);
                mStandardFolders.insert(stFolder, item);

                changedIndexes.append(getIndexByStandartFolder(stFolder));
            }
        }

        foreach (const QModelIndex& index, changedIndexes)
            emit dataChanged(index,index);
    }
}

void FoldersModel::setAccountId(const int accountId)
{
    mAccountId = accountId;
    initModel();
}

QString FoldersModel::getStandardFolderName(const int folder)
{
    if (!mStandardFolders.contains(QMailFolder::StandardFolder(folder)))
    {
        qCritical() << Q_FUNC_INFO << "there is no" << folder<< "folder";
        return QString();
    }
    return mStandardFolders.value(QMailFolder::StandardFolder(folder))->getStandardFolderName();
}

int FoldersModel::getStandardFolderByIndex(const QModelIndex& index) const
{
    int row = index.row();
    if ((row >= 0) && (row < mStandardFoldersEnum.count())) {
        return mStandardFoldersEnum.at(row);
    }
    qWarning() << Q_FUNC_INFO << "there is no " << row << "row in the folders list";
    return -1;
}

QModelIndex FoldersModel::getIndexByStandartFolder(const QMailFolder::StandardFolder& stdFolder) const
{
    return index(mStandardFoldersEnum.indexOf(stdFolder),0);
}


QModelIndex FoldersModel::index(int row, int column, const QModelIndex &parent) const
{
    return ((row >= 0 && column >= 0) && row < rowCount(parent))
            ? createIndex(row, column, parent.row())
            : QModelIndex();
}

int FoldersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mStandardFoldersEnum.count();
}

int FoldersModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex FoldersModel::parent(const QModelIndex &idx) const
{
    Q_UNUSED(idx);
    return QModelIndex();
}

QVariant FoldersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int retVal = getStandardFolderByIndex(index);
    QMailFolder::StandardFolder stFolder;
    if (retVal != -1)
        stFolder = QMailFolder::StandardFolder(retVal);
    else
        return QVariant();

    switch(role)
    {
    case standardNameRole:
    {
        if (mStandardFolders.contains(stFolder))
        {
            qWarning() << Q_FUNC_INFO << mStandardFolders.value(stFolder)->getStandardFolderName();
            return mStandardFolders.value(stFolder)->getStandardFolderName();
        }
        qCritical() << Q_FUNC_INFO << "Ouch, something nasty is happened, there is no folder" << stFolder << " in Hash";
        return QVariant();
    }
    case bindedFolderNameRole:
    {
        if (mStandardFolders.contains(stFolder))
        {
            return mStandardFolders.value(stFolder)->getDisplayName();
        }
        qCritical() << Q_FUNC_INFO << "Ouch, something nasty is happened, there is no folder" << stFolder << " in Hash";
        return QVariant();
    }
    case standardFolderRole:
    {
        return stFolder;
    }
    case iconForStandartFolderRole:
    {
        if (mStandardFolders.contains(stFolder))
        {
            return "qrc:/" + mStandardFolders.value(stFolder)->getIconForFolder();
        }
        qCritical() << Q_FUNC_INFO << "Ouch, something nasty is happened, there is no folder" << stFolder << " in Hash";
        return QVariant();
    }
    default:
        break;
    }

    return QVariant();
}

//----------------------------------------------------------------------------------------------------------
// class AccountsFoldersModel

AccountsFoldersModel::AccountsFoldersModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[folderDisplayNameRole] = "name"; // SelectionDialog will be really glad to get the "name" property :)
    setRoleNames(roles);
}

void AccountsFoldersModel::initModel()
{
    mFolders.clear();
    QMailAccountId accountId(mAccountId);
    if (accountId.isValid())
    {
        mFolders = QMailStore::instance()->queryFolders(QMailFolderKey::parentAccountId(accountId));
    }

    reset();
}

void AccountsFoldersModel::setAccountId(const int accountId)
{
    mAccountId = accountId;
    initModel();
}

int AccountsFoldersModel::getIndexOfStandartFolder(int folderType) const
{
    QMailAccountId accountId(mAccountId);
    if (!accountId.isValid())
    {
        qWarning() << Q_FUNC_INFO << accountId.toULongLong() << "account is invalid!";
        return -1;
    }
    QMailAccount account(accountId);

    QMailFolderId folderId = account.standardFolder(QMailFolder::StandardFolder(folderType));

    return mFolders.indexOf(folderId);
}

void AccountsFoldersModel::saveStandardFolder(int folderType, int folderIndex)
{
    QMailAccountId accountId(mAccountId);
    if (!accountId.isValid())
    {
        qWarning() << Q_FUNC_INFO << accountId.toULongLong() << "account is invalid!";
        return;
    }

    QMailAccount account(accountId);
    if ( (folderIndex >= 0) && (folderIndex < mFolders.count()))
    {
        QMailFolderId fId(mFolders.at(folderIndex));
        if (fId.isValid())
        {
            account.setStandardFolder(QMailFolder::StandardFolder(folderType), fId);
            if (!QMailStore::instance()->updateAccount(&account))
            {
                qWarning() << Q_FUNC_INFO << "unable to update account";
            }
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "ouch, it seems that folder id is invalid";
        }
    }
    else
        qWarning() << Q_FUNC_INFO << "ouch, it seems that folder index is invalid!";
}

QMailFolderId AccountsFoldersModel::getIdFromIndex(const QModelIndex &index) const
{
    int row = index.row();
    if ((row >= 0) && (row < mFolders.count())) {
        return mFolders.at(row);
    }
    qWarning() << Q_FUNC_INFO << "there is no" << row << "row in the folders list";
    return QMailFolderId();
}

QModelIndex AccountsFoldersModel::index(int row, int column, const QModelIndex &parent) const
{
    return ((row >= 0 && column >= 0) && row < rowCount(parent))
            ? createIndex(row, column, parent.row())
            : QModelIndex();
}

int AccountsFoldersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mFolders.count();
}

int AccountsFoldersModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex AccountsFoldersModel::parent(const QModelIndex &idx) const
{
    Q_UNUSED(idx);
    return QModelIndex();
}

QVariant AccountsFoldersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QMailFolderId folderId = getIdFromIndex(index);

    if (!folderId.isValid())
        return QVariant();

    switch(role)
    {
    case folderDisplayNameRole:
    {
        return QMailFolder(folderId).displayName();
    }
    default:
        break;
    }

    return QVariant();
}

