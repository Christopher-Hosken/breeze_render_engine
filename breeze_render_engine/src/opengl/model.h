#ifndef MODEL_H
#define MODEL_H

#include "../common.h"
#include "mesh.h"
#include "shader.h"
#include "material.h"

class Model : protected QOpenGLExtraFunctions {
public:
	std::string name;
	QVector3D location = QVector3D(0.0f, 0.0f, 0.0f);
	QVector3D rotation = QVector3D(0.0f, 0.0f, 0.0f);
	QVector3D scale = QVector3D(1.0f, 1.0f, 1.0f);

	Model() {
		genID();
	}

	Model(std::string n) {
		initializeOpenGLFunctions();
		name = n;
		genID();
	}

	void genID() {
		id = rand();
	}
	
	void draw(Shader &shader, DrawType draw) {
		QSettings settings;
		QColor selectedColor = settings.value("color/selected").value<QColor>();
		QColor wireDefaultColor = settings.value("color/wireDefault").value<QColor>();

		shader.use();
		shader.setMat4("model", getModelMatrix());
		if (draw == ID) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			float rID = (id & 0x000000FF) >> 0;
			float gID = (id & 0x0000FF00) >> 8;
			float bID = (id & 0x00FF0000) >> 16;

			shader.setColor("color", QColor(rID, gID, bID));
		}
		if (draw == DEFAULT) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			shader.setMaterial(material);

			if (selected) {
				float r = (material.color.red() + selectedColor.red()) / 2.0f;
				float g = (material.color.green() + selectedColor.green()) / 2.0f;
				float b = (material.color.blue() + selectedColor.blue()) / 2.0f;
				QColor newColor = QColor(r, g, b);
				shader.setColor("material.color", newColor);
			}
		}
		else if (draw == WIRE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (selected) {
				shader.setColor("color", selectedColor);
				glLineWidth(5.0f);
			}
			else {
				shader.setColor("color", wireDefaultColor);
			}
			glLineWidth(1.0f);
		}
		else if (draw == SHADED) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		}


		glBindVertexBuffer(0, mesh.getVBO(), 0, sizeof(Vertex));
		glBindVertexBuffer(1, mesh.getVBO(), offsetof(Vertex, normal), sizeof(Vertex));
		glBindVertexBuffer(2, mesh.getVBO(), offsetof(Vertex, uv), sizeof(Vertex));
		glDrawArrays(GL_TRIANGLES, 0, mesh.data.size());
	}

	QMatrix4x4 getModelMatrix() {
		QMatrix4x4 scaleMatrix = QMatrix4x4();
		scaleMatrix.setToIdentity();
		scaleMatrix.scale(scale - QVector3D(1.0f, 1.0f, 1.0f));

		QMatrix4x4 rotMatrix = QMatrix4x4();
		rotMatrix.setToIdentity();
		
		QMatrix4x4 locMatrix = QMatrix4x4();
		locMatrix.setToIdentity();
		locMatrix.translate(location);

		return scaleMatrix * rotMatrix + locMatrix;
	}

	void dispose() {
		mesh.dispose();
	}

	unsigned int id = 1;
	bool selected = false;

	ObjectType type = SOLID;
	Mesh mesh;
	Material material = {
		QColor(255, 255, 255),
		0.5f,
		0.5f
	};
};

#endif // !MODEL_H