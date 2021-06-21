#include "simlib.h"
#include "graph.h"
#include "normal.h"

#define MAX_NODE_NUM  200
#define MAX_GRAPH_NUM 100

enum EVENT {
  EVENT_ARRIVAL=1,
  EVENT_END_SIMULATION
};

enum STREAM {
  STREAM_CHANNEL=1,
  STREAM_FUND,
  STREAM_ARRIVAL, 
  STREAM_PAYMENT,
  STREAM_ROUTE_FROM,
  STREAM_ROUTE_TO,
  STREAM_NORMAL_1,
  STREAM_NORMAL_2
};

// non-simlib global variables
int num_node, num_graph, fee_policy;
Graph* ln[MAX_GRAPH_NUM];
double mean_interarrival, length_simulation, mean_value, std_value;
int sim_progess;
vector<float> time_record;
vector<float*> imbalance_record;
vector<int*> fail_record_path;
vector<int*> fail_record_fund;

// file
FILE *infile;
ofstream imbalance_csv;
ofstream fail_csv;

void init(char*);
void arrive(void);
void report(void);
void generate_payment();

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "please input the testing file name.\n"
         << "ex. ./bin/main ./test/test10.in" << endl;
    exit(1);
  }

  // initialize simlib
  init_simlib();
  maxatr = 7;

  // initalize all settings before simulation start
  init(argv[1]);

  // start simulation
  cout << "\r" << "0% " << "   " << flush;
  generate_payment();
  event_schedule(expon(mean_interarrival, STREAM_ARRIVAL), EVENT_ARRIVAL);
  transfer[3] = 1; // normal end
  event_schedule(length_simulation, EVENT_END_SIMULATION);
  do {
    timing();
    switch (next_event_type) {
      case EVENT_ARRIVAL:
        arrive();
        break;
      case EVENT_END_SIMULATION:
        report();
        break;
    }
  } while (next_event_type != EVENT_END_SIMULATION);

  // delete the allocated memory
  for (int i = 0; i < num_graph; i += 1) {
    delete ln[i];
  }
  for (int i = 0; i < imbalance_record.size(); i += 1) {
    delete [] imbalance_record[i];
    delete [] fail_record_path[i];
    delete [] fail_record_fund[i];
  }

  // close files
  fclose(infile);
  imbalance_csv.close();
  fail_csv.close();
  return 0;
}

void init(char* input_name) {
  // open input and ouput files
  infile = fopen(input_name,  "r");
  string infile_name(input_name);
  size_t lastindex = infile_name.find_last_of("."); 
  infile_name = infile_name.substr(0, lastindex);
  string outfile_imb = infile_name + "_imbalance.csv";
  string outfile_stc = infile_name + "_fail.csv";
  imbalance_csv.open(outfile_imb);
  fail_csv.open(outfile_stc);

  // clear parameter
  sim_progess = 0;
  fail_record_path.clear();
  fail_record_fund.clear();
  time_record.clear();
  imbalance_record.clear();

  // read file
  double prob_channel, min_channel_fund, max_channel_fund;
  fscanf(infile, "%d %d %lf %lf", &num_node, &num_graph, &mean_interarrival, &length_simulation);
  fscanf(infile, "%lf %lf %lf %lf %lf", &prob_channel, &min_channel_fund, &max_channel_fund, &mean_value, &std_value);
  fscanf(infile, "%d", &fee_policy);
  if(num_node > MAX_NODE_NUM) {
    cout << "\nNode number overflow" << endl;
    exit(1);
  }
  if (num_graph > MAX_GRAPH_NUM) {
    cout << "\nGraph number overflow" << endl;
    exit(1);
  }
  if (fee_policy != 1 && fee_policy != 2) {
    cout << "\nfee policy error" << endl;
    exit(1);
  }

  // create graph
  for (int i = 0; i < num_graph; i += 1) {
    ln[i] = new Graph(num_node, prob_channel, min_channel_fund, max_channel_fund, STREAM_CHANNEL, STREAM_FUND);
  }

  // initial records
  time_record.push_back(0.0);
  float* im_record = new float[num_graph];
  int* f_record_p = new int[num_graph];
  int* f_record_f = new int[num_graph];
  for (int i = 0; i < num_graph; i += 1) {
    im_record[i] = ln[i]->getImbalanceRatio();
    f_record_p[i] = 0;
    f_record_f[i] = 0;
  }
  imbalance_record.push_back(im_record);
  fail_record_path.push_back(f_record_p);
  fail_record_fund.push_back(f_record_f);
}

void generate_payment() {
  int from = -1, to = -1;
  while (0 > from || from >= num_node || 0 > to || to >= num_node || from == to) {
    from = int(uniform(0.0, float(num_node), STREAM_ROUTE_FROM));
    to = int(uniform(0.0, float(num_node), STREAM_ROUTE_TO));
  }
  transfer[3] = from;
  transfer[4] = to;
  transfer[5] = truncated_normal(mean_value, std_value, 0.0, 2.0 * mean_value, STREAM_NORMAL_1, STREAM_NORMAL_2);
}

void arrive(void) {
  int from = transfer[3];
  int to = transfer[4];
  double amount = transfer[5];

  // every graph deal with same arrival
  float* im_record = new float[num_graph];
  int* f_record_p = new int[num_graph];
  int* f_record_f = new int[num_graph];
  for (int i = 0; i < num_graph; i += 1) {
    int send_event = ln[i]->sendPayment(from, to, amount, fee_policy);
    if (send_event == 1) {
      f_record_f[i] = fail_record_fund[fail_record_fund.size() - 1][i] + 1;
      f_record_p[i] = fail_record_path[fail_record_path.size() - 1][i];
    } else if (send_event == 2) {
      f_record_f[i] = fail_record_fund[fail_record_fund.size() - 1][i];
      f_record_p[i] = fail_record_path[fail_record_path.size() - 1][i] + 1;
    } else if (send_event == 0) {
      f_record_f[i] = fail_record_fund[fail_record_fund.size() - 1][i];
      f_record_p[i] = fail_record_path[fail_record_path.size() - 1][i];
    }
    im_record[i] = ln[i]->getImbalanceRatio();
  }
  time_record.push_back(sim_time);
  imbalance_record.push_back(im_record);
  fail_record_fund.push_back(f_record_f);
  fail_record_path.push_back(f_record_p);

  // show the simulation progess
  if (int(sim_time * 100 / length_simulation) != sim_progess) {
    sim_progess = int(sim_time * 100 / length_simulation);
    cout << "\r" << sim_progess << "%    " << flush;
  }

  // next arrival
  generate_payment();
  event_schedule(sim_time + expon(mean_interarrival, STREAM_ARRIVAL), EVENT_ARRIVAL);
}

void report(void) {
  // write imbalance records
  imbalance_csv << "time";
  for (int i = 0; i < num_graph; i += 1)
    imbalance_csv << ",graph" << i + 1 << " imbalance ratio";
  imbalance_csv << ",average";
  for (int i = 0; i < imbalance_record.size(); i += 1) {
    imbalance_csv << "\r\n" << time_record[i];
    float total = 0.0;
    for (int j = 0; j < num_graph; j += 1) {
      imbalance_csv << "," << imbalance_record[i][j];
      total += imbalance_record[i][j];
    }
    imbalance_csv << "," << total / num_graph;
  }
  imbalance_csv << endl;

  // write fail records
  fail_csv << "time";
  for (int i = 0; i < num_graph; i += 1)
    fail_csv << ",graph" << i + 1 << " path fail";
  fail_csv << ",average,,time";
  for (int i = 0; i < num_graph; i += 1)
    fail_csv << ",graph" << i + 1 << " fund fail";
  fail_csv << ",average";
  for (int i = 0; i < fail_record_fund.size(); i += 1) {
    fail_csv << "\r\n" << time_record[i];
    float total = 0.0;
    for (int j = 0; j < num_graph; j += 1) {
      fail_csv << "," << fail_record_path[i][j];
      total += fail_record_path[i][j];
    }
    fail_csv << "," << total / num_graph << ",,";
    fail_csv << time_record[i];
    total = 0.0;
    for (int j = 0; j < num_graph; j += 1) {
      fail_csv << "," << fail_record_fund[i][j];
      total += fail_record_fund[i][j];
    }
    fail_csv << "," << total / num_graph;
  }
  fail_csv << endl;
}
