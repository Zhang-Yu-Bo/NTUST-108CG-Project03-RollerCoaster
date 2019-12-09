#ifndef MODEL_H
#define MODEL_H

#include <QtCore/QString>
#include <QtCore/QVector>

#include <math.h>

#include "point3d.h"

class Model
{
public:
	Model() {}
	Model(const QString &filePath, int s, Point3d p);

	void moveModel(int, Point3d);
	void render(bool wireframe = false, bool normals = false) const;

	QString fileName() const { return m_fileName; }
	int faces() const { return m_pointIndices.size() / 3; }
	int edges() const { return m_edgeIndices.size() / 2; }
	int points() const { return m_points.size(); }

private:
	Point3d position;
	Point3d boundsMin;
	Point3d boundsMax;
	Point3d bounds;
	qreal scale;
	QVector<Point3d> m_points_copy;

	QString m_fileName;
	QVector<Point3d> m_points;
	QVector<Point3d> m_normals;
	QVector<int> m_edgeIndices;
	QVector<int> m_pointIndices;
};

#endif