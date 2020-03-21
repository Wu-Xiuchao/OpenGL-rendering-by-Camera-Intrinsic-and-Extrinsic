#include <fstream>
#include <vector>

#include <iostream>

// 分割字符串
std::vector<std::string> split(std::string str, char c)
{
	std::vector<std::string> res;
	std::string accumulate_str = "";
	for(int i=0; i<str.size(); i++)
	{
		if(str[i] == c)
		{
			if (accumulate_str != "")
				res.emplace_back(accumulate_str);
			accumulate_str = "";
		}else{
			accumulate_str += str[i];
		}
	}
	res.emplace_back(accumulate_str);
	return res;
}


template <typename T>
void free(std::vector<T> list)
{
	for (int i=0; i<list.size(); i++){
		delete [] list[i];
	}
}

class Ply_Loader{
public:
	Ply_Loader(){}
	void load_model(std::string path)
	{
		std::vector<float*> vertex_list;
		std::vector<int*> face_info;

		std::ifstream infile;
		infile.open(path);
		std::string data = "";

		int num_vertex = 0, num_face = 0;

		while(data != "end_header")
		{
			getline(infile, data);
			if (data == "") continue;
			std::vector<std::string> data_list = split(data, ' ');
			if (data_list[0] == "element")
			{
				if (data_list[1] == "vertex"){
					num_vertex = stoi(data_list[2]);
				}else if (data_list[1] == "face"){
					num_face = stoi(data_list[2]);
				}
			}	
		}
		for (int i=0; i<num_vertex; i++)
		{
			getline(infile, data);
			std::vector<std::string> data_list = split(data, ' ');
			float* v_arr = new float[3];
			for (int j=0; j<3; j++)
			{
				v_arr[j] = stof(data_list[j]);
			}
			vertex_list.emplace_back(v_arr);
		}
		this->num_vertex = num_face * 3;
		this->len_vertices = this->num_vertex * 5;
		this->vertices = new GLfloat[this->len_vertices];
		int index = 0;
		for (int i=0; i<num_face; i++)
		{
			getline(infile, data);
			std::vector<std::string> data_list = split(data, ' ');
			int v1 = stoi(data_list[1]);
			int v2 = stoi(data_list[2]);
			int v3 = stoi(data_list[3]);
			float t1 = stof(data_list[5]);
			float t2 = stof(data_list[6]);
			float t3 = stof(data_list[7]);
			float t4 = stof(data_list[8]);
			float t5 = stof(data_list[9]);
			float t6 = stof(data_list[10]);
			for (int j=0; j<5; j++){
				if (j < 3){
					this->vertices[index] = vertex_list[v1][j];
				}else if (j == 3){
					this->vertices[index] = t1;
				}else{
					this->vertices[index] = t2;
				}
				std::cout << this->vertices[index] << " ";
				index += 1;
			}
			std::cout << std::endl;
			for (int j=0; j<5; j++){
				if (j < 3){
					this->vertices[index] = vertex_list[v2][j];
				}else if (j == 3){
					this->vertices[index] = t3;
				}else{
					this->vertices[index] = t4;
				}
				std::cout << this->vertices[index] << " ";
				index += 1;
			}
			std::cout << std::endl;
			for (int j=0; j<5; j++){
				if (j < 3){
					this->vertices[index] = vertex_list[v3][j];
				}else if (j == 3){
					this->vertices[index] = t5;
				}else{
					this->vertices[index] = t6;
				}
				std::cout << this->vertices[index] << " ";
				index += 1;
			}
			std::cout << std::endl;
		}
		free(vertex_list);
		free(face_info);
		std::cout << "导入 ply 模型 成功！" << std::endl;
	}
	GLfloat* get_vertices(){
		return this->vertices;
	}
	GLint get_len(){
		return this->len_vertices;
	}
	GLint get_num_vertex(){
		return this->num_vertex;
	}
private:
	GLfloat* vertices; //  顶点数组， 包括顶点坐标，贴图坐标
	GLint len_vertices; // 顶点数组的长度
	GLint num_vertex; // 顶点数目 (面片数 x 3)
};