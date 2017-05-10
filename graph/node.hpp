#ifndef NODE_HPP
#define NODE_HPP

#include <QObject>
#include <QString>
#include <QStringList>

class Node : public QObject
{
    Q_OBJECT

private:

    void init( QString id,
               QString title,
               QString icon,
               QStringList children,
               QString text);

    QString id;
    QString title;
    QString icon;
    QStringList children;
    QString text;

public:

    explicit Node( QString id       = "",
                   QString title    = "",
                   QString icon     = "",
                   QStringList children = QStringList(),
                   QString text     = "",
                   QObject *parent  = NULL );
    ~Node();

    QString getId() const;
    void setId( const QString &value );

    QString getTitle() const;
    void setTitle( const QString &value );

    QString getIcon() const;
    void setIcon( const QString &value );

    QStringList getChildren() const;
    void setChildren( const QStringList &value );

    QString getText() const;
    void setText( const QString &value );
};

#endif // NODE_HPP

