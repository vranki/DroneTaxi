#ifndef GAUGEBAND_H
#define GAUGEBAND_H

#include <QGraphicsWidget>
#include <QGraphicsItem>
#include <QPainter>

#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define SCROLL 0x10
#define SCROLL_UP 0x11
#define SCROLL_RIGHT 0x12
#define SCROLL_DOWN 0x13
#define SCROLL_LEFT 0x14

class Gauge : public QGraphicsWidget
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString format READ format WRITE setFormat)
    Q_PROPERTY(int gaugeStyle READ gaugeStyle WRITE setGaugeStyle)
    Q_PROPERTY(float value2 READ value2 WRITE setValue2)
    Q_PROPERTY(float value READ value WRITE setValue)
    Q_PROPERTY(float low READ low WRITE setLow)
    Q_PROPERTY(float high READ high WRITE setHigh)
    Q_PROPERTY(float lowRed READ lowRed WRITE setLowRed)
    Q_PROPERTY(float highRed READ highRed WRITE setHighRed)
    Q_PROPERTY(float tickIncrement READ tickIncrement WRITE setTickIncrement)

public:
    explicit Gauge(QGraphicsItem *parent = 0);


    virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,
                               QWidget *widget);

    QRectF boundingRect() const;

    QString name() const { return m_name;};
    void setName(const QString &name_) {m_name=name_;}
    QString format() const { return m_format;};
    void setFormat(const QString &name_) {m_format=name_;}
    void setGaugeStyle(int val_) { m_style=val_;};
    int  gaugeStyle() {return m_style;};
    void setValue(float val_);
    float value();
    void setValue2(float val_) { m_value2=val_;};
    float value2() {return m_value2;};
    void setLow(float val_) { m_low=val_;};
    float low() {return m_low;};
    void setHigh(float val_) { m_high=val_;};
    float high() {return m_high;};
    void setLowRed(float val_) { m_lowred=val_;};
    float lowRed() {return m_lowred;};
    void setHighRed(float val_) { m_highred=val_;};
    float highRed() {return m_highred;};
    void setTickIncrement(float val_) { m_tickIncrement=val_;};
    float tickIncrement() {return m_tickIncrement;};

signals:

public slots:
public:
    QString m_name;
    QString m_format;
    int m_tickIncrement;  // Tick increment
    int m_tspc;  // Tick spacing
    int   m_style;
    float m_value;  // primary value,pitch in horizon
    float m_value2; // Roll in horizon
    float m_low;        // low value to display
    float m_high;       // High value to display
    float m_lowred;     // low red limit
    float m_lowyellow;  // low yellow limit
    float m_highyellow; // higest yellow limit
    float m_highred;
};

#endif // GAUGE_H
