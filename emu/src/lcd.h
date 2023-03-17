/*
 * Author: Pawel Jablonski
 * E-mail: pj@xirx.net
 * WWW: xirx.net
 * GIT: git.xirx.net
 *
 * License: You can use this code however you like
 * but leave information about the original author.
 * Code is free for non-commercial and commercial use.
 */

#ifndef LCD_H
#define LCD_H

#include <QObject>
#include <QVector>

#ifdef CHAR_WIDTH
#undef CHAR_WIDTH
#endif

//! This class contains LCD functions and buffer
class LCD : public QObject
{
	Q_OBJECT

	public:
		static const int CHAR_WIDTH = 8; //!< Width of the char in pixels
		static const int CHAR_HEIGHT = 8; //!< Height of the char in pixels

		static const int WIDTH = 40; //!< Chars per row
		static const int HEIGHT = 30; //!< Chars per column

		static const int COLOR_MASK = 0x0f; //!< Char color mask, two colors per byte
		static const int COLOR_SHIFT = 4; //!< Char color shift, background color is high 4 bits, foreground color is low 4 bits

		//! LCD buffer
		struct Buffer
		{
			QVector<QVector<unsigned char>> charData = QVector<QVector<unsigned char>>(HEIGHT, QVector<unsigned char>(WIDTH)); //!< Char buffer
			QVector<QVector<unsigned char>> colorData = QVector<QVector<unsigned char>>(HEIGHT, QVector<unsigned char>(WIDTH)); //!< Background and foreground color buffer
		};

		LCD(QObject *parent = nullptr);

		void reset();

		void refresh();

		void clear();
		void setChar(unsigned char c, int x, int y, unsigned char charColor, unsigned char backgroundColor);

		void scroll();

		void update();

	private:
		QVector<Buffer> buffer = { Buffer(), Buffer() }; //!< First buffer is using as canvas for painting, the second one is a copy ready to show

	signals:
		void updateBufferSignal(LCD::Buffer buffer);
};

#endif
