#ifndef SCENE_H
#define SCENE_H
    struct dmx_buffer{
        public:
            int size;
            unsigned char channel[513];
    };
    class scene{
        public:
            bool writable;
            int size;
            double channel[512];
            scene();
            scene(int);
            scene(std::string, int, bool);
            void load(std::string, int, bool);
            void save(std::string);
            void sround();
            dmx_buffer to_dmx();
            void print();
            void print_g();
            scene operator+(scene);
            scene operator-(scene);
            scene operator*(int);
            scene operator/(int);
    };
#endif
