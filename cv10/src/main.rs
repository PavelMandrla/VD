extern crate kiss3d;
extern crate nalgebra as na;

use std::collections::VecDeque;
use na::{Vector3, Point3, Translation3};
use kiss3d::window::Window;
use kiss3d::light::Light;
use kiss3d::scene::SceneNode;
use rand::Rng;

struct Node {
    translation: Translation3<f32>,
    force: Vector3<f32>,
    sphere: SceneNode,
}

impl Node {
    fn translate(&mut self) {
        self.sphere.set_local_translation(self.translation);
    }

    fn use_the_force(&mut self, t:f32) {
        let delta: f32 = 0.1;
        let df = self.force.norm();
        if df > 0.0 {
            let alt = t * df;
            let m = if delta < alt { delta } else { alt };
            let ds = (self.force * m) / df;

            self.translation.x += ds[0];
            self.translation.y += ds[1];
            self.translation.z += ds[2];
        }
    }

    fn get_position(&self) -> Point3<f32> {
        Point3::from_slice(&[
            self.translation.x,
            self.translation.y,
            self.translation.z,
        ])
    }
}

struct Vertex {
    node_1: usize,
    node_2: usize,
}

struct Graph {
    incidence_mat: Vec<Vec<bool>>,
    nodes: Vec<Node>,
    vertices: Vec<Vertex>,

    k: f32,
    t: f32,
}

impl Graph {
    fn would_be_cycle(node1: usize, node2: usize, g: &Graph) -> bool {
        let mut visited: Vec<usize> = Vec::new();
        let mut queue: VecDeque<usize> = VecDeque::new();

        queue.push_back(node1);
        while !queue.is_empty() {
            let tmp = queue.pop_front().unwrap();
            if visited.contains(&tmp) { continue; }
            if g.incidence_mat[tmp][node2] { return true; }

            for i in 0..g.incidence_mat[tmp].len() {
                if g.incidence_mat[tmp][i] {
                    queue.push_back(i)
                }
            }
            visited.push(tmp);
        }

        false
    }

    fn new(nodes: usize, p_vertex: f32, k: f32, window: &mut Window) -> Graph {
        let mut result = Graph {
            incidence_mat: vec![vec![false; nodes]; nodes],
            nodes: Vec::new(),
            vertices: Vec::new(),
            k: k,
            t: 1.0,
        };
        let mut rng = rand::thread_rng();

        // GENERATE RANDOM NODE POSITIONS
        for _i in 0..nodes {
            let mut sphere = window.add_sphere(0.5);
            sphere.set_color(0.0, 1.0, 0.0);

            result.nodes.push( Node {
                translation: Translation3::new(
                    rng.gen_range(-10.0..10.0),
                    rng.gen_range(-10.0..10.0),
                    rng.gen_range(-10.0..10.0),
                ),
                force: Vector3::new(0.0, 0.0, 0.0),
                sphere: sphere,
            });
        }

        // GENERATE VERTICES
        for i in 0..nodes {
            result.incidence_mat[i][i] = true;
            for j in i+1..nodes {
                //if i == j { continue; }
                if rng.gen_range(0.0..1.0) <= p_vertex {
                    if !Graph::would_be_cycle(i, j, &result) {
                        result.vertices.push(Vertex {
                            node_1: i,
                            node_2: j,
                        });
                        result.incidence_mat[i][j] = true;
                        result.incidence_mat[j][i] = true;
                    }
                }
            }
        }
        result
    }

    fn f_a(&self, i: usize, j: usize) -> Vector3<f32> {
        if self.incidence_mat[i][j] {
            let p_i = &self.nodes[i].get_position();
            let p_j = &self.nodes[j].get_position();
            (p_i - p_j).norm() * (p_j - p_i) / self.k
        } else {
            Vector3::new(0.0, 0.0, 0.0)
        }
    }

    fn f_r(&self, i: usize, j: usize) -> Vector3<f32> {
        if i == j {
            Vector3::new(0.0, 0.0, 0.0)
        } else {
            let p_i = &self.nodes[i].get_position();
            let p_j = &self.nodes[j].get_position();

            -self.k.powi(2) *  (p_j - p_i) / (p_i - p_j).norm().powi(2)
        }
    }

    fn f_centre(&self, i: usize) -> Vector3<f32> {
        let p_i = &self.nodes[i].get_position();
        let p_j: &Point3<f32> = &Point3::from_slice(&[0.0, 0.0, 0.0]);
        (p_i - p_j).norm() * (p_j - p_i) / self.k
    }

    fn update(&mut self) {
        for i in 0..self.nodes.len() {
            // RESET THE FORCE ON THE NODE
            self.nodes[i].force = Vector3::new(0.0, 0.0, 0.0);

            for j in 0..self.nodes.len() {
                // CALCULATE FORCES
                let f_r = self.f_r(i, j);
                let f_a = self.f_a(i, j);
                self.nodes[i].force += f_r;
                self.nodes[i].force += f_a;
            }
            let f_c = self.f_centre(i);
            self.nodes[i].force += f_c;
        }

        for node in &mut self.nodes {
            node.use_the_force(self.t);
            node.translate();
        }
        self.t -= self.t * 0.01;
    }
}

fn main() {
    let mut window = Window::new_with_size("Kiss3d: cube", 900, 900);

    let mut graph = Graph::new(100, 0.07, 5.0, &mut window);

    window.set_light(Light::StickToCamera);

    while window.render() {
        graph.update();

        let color = Point3::from_slice(&[0.0, 1.0, 0.0]);
        for v in &graph.vertices {
            let p1= graph.nodes[v.node_1].get_position();
            let p2= graph.nodes[v.node_2].get_position();

            window.draw_line(&p1, &p2, &color);
        }
    }
} 