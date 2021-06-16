#include "simlib.h"
#include "graph.h"
#include "normal.h"

#define MAX_NODE_NUM  200

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
int num_node;
Graph* ln;
//     num_machines_busy[MAX_NUM_STATIONS + 1];
double mean_interarrival, length_simulation, mean_value;
//       prob_distrib_station[3][3],
//       min_service_time[MAX_NUM_STATIONS + 1],
//       max_service_time[MAX_NUM_STATIONS + 1];
FILE *infile, *outfile, *outfile_csv;

void init(void);
void arrive(void);
void report(void);
void generate_payment();

int main() {
  // initalize all settings before simulation start
  init();
  // cout << *ln << endl;
  // cout << ln->getImbalanceRatio() << endl;
  // ln->sendPayment(2, 199, 0.001, FEE_OPTIMIZED);
  cout << ln->getImbalanceRatio() << endl;

  // initialize simlib  
  init_simlib();
  maxatr = 7;

  // start simulation
  // cout << transfer[3] << " " << transfer[4] << " " << transfer[5] << endl;
  generate_payment();
  // cout << transfer[3] << " " << transfer[4] << " " << transfer[5] << endl;
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
  delete ln;
  fclose(infile);
  return 0;
}

void init(void) {
  // open input and ouput files
  infile  = fopen("test/test10.in",  "r");
  // outfile = fopen("test/hw1.out", "w");
  // outfile_csv = fopen("test/hw1.csv", "w");
  double prob_channel, min_channel_fund, max_channel_fund;
  fscanf(infile, "%d %lf %lf", &num_node, &mean_interarrival, &length_simulation);
  fscanf(infile, "%lf %lf %lf %lf", &prob_channel, &min_channel_fund, &max_channel_fund, &mean_value);
  if(num_node > MAX_NODE_NUM) {
    cout << "\nNode number overflow" << endl;
    exit(1);
  }
  ln = new Graph(num_node, prob_channel, min_channel_fund, max_channel_fund, STREAM_CHANNEL, STREAM_FUND);
}

void generate_payment() {
  int from = -1, to = -1;
  while (0 > from || from >= num_node || 0 > to || to >= num_node || from == to) {
    from = int(uniform(0.0, float(num_node), STREAM_ROUTE_FROM));
    to = int(uniform(0.0, float(num_node), STREAM_ROUTE_TO));
  }
  transfer[3] = from;
  transfer[4] = to;
  transfer[5] = truncated_normal(mean_value, sqrt(0.0012), 0.0, 2.0 * mean_value, STREAM_NORMAL_1, STREAM_NORMAL_2);
}

void arrive(void) {
  int from = transfer[3];
  int to = transfer[4];
  double amount = transfer[5];
  // cout << from << " " << to << endl;
  if (!ln->sendPayment(from, to, amount, FEE_OPTIMIZED)) {
    cout << "No path" << endl;
    // event_schedule(sim_time + 100, EVENT_ARRIVAL);
  }
  // else
    // cout << ln->getImbalanceRatio() << endl;
  if (int(sim_time) % 1000 == 0) {
    cout << sim_time << " " << ln->getImbalanceRatio() << endl;
  }
  generate_payment();
  // cout << transfer[3] << " " << transfer[4] << " " << transfer[5] << endl;
  event_schedule(sim_time + expon(mean_interarrival, STREAM_ARRIVAL), EVENT_ARRIVAL);

  // for (int i = 0; i < 20; i += 1) {
  //   if (!ln->sendPayment(2, 199, 0.01, FEE_OPTIMIZED))
  //     cout << i << " No path" << endl;
  // }
}

void report(void) {
  cout << ln->getImbalanceRatio() << endl;
  cout << "end" << endl;
}
