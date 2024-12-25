import ROOT
import pandas as pd
import torch
from torch_geometric.data import Data, Dataset
import numpy as np

ecal_id_xy = {0: [-127, -127], 1: [-127, -63.5], 2: [-127, 0], 3: [-127, 63.5], 4: [-127, 127], 5: [-63.5, -127], 6: [-63.5, -63.5], 7: [-63.5, 0], 8: [-63.5, 63.5], 9: [-63.5, 127], 10: [0, -127], 11: [0, -63.5], 12: [0, 0], 13: [0, 63.5], 14: [0, 127], 15: [63.5, -127], 16: [63.5, -63.5], 17: [63.5, 0], 18: [63.5, 63.5], 19: [63.5, 127], 20: [127, -127], 21: [127, -63.5], 22: [127, 0], 23: [127, 63.5], 24: [127, 127]}


def calculate_attributes(x,y,z,e):
    r_t = np.sqrt(x**2 + y**2) # radius distance
    theta = np.arctan2(r_t,z)
    deta = -np.log(np.tan(theta/2))
    dphi = np.arctan2(y, x)
    pt = e
    e = e
    ptrel = e
    erel = e
    dR = np.sqrt(deta**2 + dphi**2)
    return [deta, dphi, pt, e, ptrel, erel, dR]
def fill_attributes(deta_list, dphi_list, pt_list, e_list, ptrel_list, erel_list, dR_list, x, y, z, e):
    attrs = calculate_attributes(x,y,z,e)
    deta_list.append(float(attrs[0]))
    dphi_list.append(float(attrs[1]))
    pt_list.append(float(attrs[2]))
    e_list.append(float(attrs[3]))
    ptrel_list.append(float(attrs[4]))
    erel_list.append(float(attrs[5]))
    dR_list.append(float(attrs[6]))
    

file = ROOT.TFile.Open("test.root")
tree = file.Get("vtree")
if not tree:
    print("Error: TTree 'vtree' not found in the file.")
    exit(1)

index=0
data = []
data_regression = []
for event in tree:
    if(index%1000==0):
        print(index)
    deta_list=[]
    dphi_list=[]
    pt_list=[]
    e_list=[]
    ptrel_list=[]
    erel_list=[]
    dR_list=[]
    #converter hits
    fill_attributes(deta_list, dphi_list, pt_list, e_list, ptrel_list, erel_list, dR_list, float(0.), float(0.), float(-251.8), event.conv_e)
    #tracker hits
    for i in range(len(event.tracker_hitx)):
        x = event.tracker_hitx[i]
        y = event.tracker_hity[i]
        z = event.tracker_hitz[i]
        hite = float(0.1)
        # append
        fill_attributes(deta_list, dphi_list, pt_list, e_list, ptrel_list, erel_list, dR_list, x, y, z, hite)
    #ecal hist
    for i in range(len(event.ecal_cellid)):
        id = event.ecal_cellid[i]
        x = ecal_id_xy[id][0]
        y = ecal_id_xy[id][1]
        z = float(0.1)
        e = event.ecal_celle[i]
    df_deta = pd.DataFrame(deta_list, columns=["deta"])
    df_dphi = pd.DataFrame(dphi_list, columns=["dphi"])
    df_pt = pd.DataFrame(pt_list, columns=["pt"])
    df_e = pd.DataFrame(e_list, columns=["e"])
    df_ptrel = pd.DataFrame(ptrel_list, columns=["ptrel"])
    df_erel = pd.DataFrame(erel_list, columns=["erel"])
    df_dR = pd.DataFrame(dR_list, columns=["dR"])

    data.append(
        Data(
            x=torch.cat(
                [
                    torch.from_numpy(df_deta.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_dphi.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_pt.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_e.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_ptrel.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_erel.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_dR.to_numpy().reshape(-1,1)),
                ],
                axis=1,
            ),
            y=torch.tensor(event.isconv).long(),
        )
    )
    data_regression.append(
        Data(
            x=torch.cat(
                [
                    torch.from_numpy(df_deta.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_dphi.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_pt.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_e.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_ptrel.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_erel.to_numpy().reshape(-1,1)),
                    torch.from_numpy(df_dR.to_numpy().reshape(-1,1)),
                ],
                axis=1,
            ),
            y=torch.tensor(event.init_E),
        )
    )
    index+=1

torch.save(data,"gnn_dataset.pt")
torch.save(data_regression,"gnn_dataset_regression.pt")
# print(df)
    # print(data)
    # print(event.tracker_hitx)
