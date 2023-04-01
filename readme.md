# Lightning Network Similation

閃電網路（lightning network）是透過區塊鏈中兩兩節點建構交易通道（payment channel），連結而成的交易網路，網路中沒有建立交易通道的節點間，也可以透過其他節點作為中間人而完成交易，交易會導致通道的中的資金分佈改變，嚴重時會出現通道資金不平衡的現象（channel imbalance），此專案透過 C++ 模擬此現象，並展示其對交易成功率的影響。

## Compile 

```
make
```

## Run simulation

- create simulation parameter file with format
```
num_node   num_graph   mean_interarrival_time    simulation_length
probobility_of_channel   minimum_channel_fund   maximum_channel_fund   mean_channel_value   standard_deviation_channel_value
fee_policy   lowest_fee   highest_fee   default_fee
```
ex.
```
  200     10         1.0   10000.0
 0.03     0.01      0.05     0.003    0.00128
 1        0.001      0.005      0.005
```

- Run simulation
```
./bin/main {parameter_file}
```

## Run simulation by existing test parameters
- parameter locate at directory `test`
```
make test_{test_dir}
```
ex. 
```
make test_standard
```