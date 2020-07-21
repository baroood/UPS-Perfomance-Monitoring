import dash
from dash.dependencies import Output, Input
import dash_core_components as dcc
import dash_html_components as html
import plotly
import random
import plotly.graph_objs as go
from collections import deque
import numpy as np
import time
import requests
import json

def get_data(uri, format="json"):
    """
        Method description:
        Gets data from the specified container(data_CIN)
        in the OneM2M framework/tree

        Parameters:
        uri : [str] URI for the parent DATA CON appended by "la" or "ol"
        fmt_ex : [str] payload format (json/XML)
    """
    headers = {
        'X-M2M-Origin': 'admin:admin',
        'Content-type': 'application/json'}

    response = requests.get(uri, headers=headers)
    #print('Return code : {}'.format(response.status_code))
    #print('Return Content : {}'.format(response.text))
    _resp = json.loads(response.text)
    return _resp["m2m:cin"]["con"],_resp["m2m:cin"]["ct"]

tempk=26
humk=65
data = {
        'Time': [],
        'Temp': [],
        'Hum' : [],
        'EM1' : [],
        'EM2' : [],
        'EM1p': [],
        'EM2p': [],
        'EnergyDiff': [],
        'Efficiency': []
    }

app = dash.Dash(__name__)
app.layout = html.Div(
    [   dcc.Graph(id='live-graph1', animate=False),
        dcc.Interval(
            id='graph-update1',
            interval=1*64*1000
        ),

    ]
)

@app.callback(Output('live-graph1', 'figure'),
              [Input('graph-update1', 'n_intervals')])
def update_graph_scatter(input_data):
    info,time=get_data("http://onem2m.iiit.ac.in/~/in-cse/in-name/Team43_UPS_performance_monitoring/pr_5_esp32_1/em/em_2_vll_avg/la")
    time = time[:4]+'-'+time[5:7]+"-"+time[6:8]+' '+time[9:11]+':'+time[11:13]+':'+time[13:]
    info=info[info.find("(")+1:info.find(")")]
    tempcel=(info.split(",")[0])
    humidity=(info.split(",")[1])
    em1p=(info.split(",")[2])
    em2p=(info.split(",")[3])
    em1=(info.split(",")[4])
    em2=(info.split(",")[5])
    ed=float(em1)-float(em2)
    ed=str(ed)
    ef = float((float(em2)/float(em1)))*100



    data['Time'].append(time)
    if tempcel>str(0) and tempcel<str(50):
        data['Temp'].append(tempcel)
        
    else:
        data['Temp'].append(tempk)
    
    if float(humidity)>0 and float(humidity)<100:
        data['Hum'].append(humidity)
        
    else:
        data['Hum'].append(humk)
    
    data['EM1'].append(em1)
    data['EM2'].append(em2)
    data['EM1p'].append(em1p)
    data['EM2p'].append(em2p)
    data['EnergyDiff'].append(ed)
    data['Efficiency'].append(ef)
    tempk=data['Temp'][-1]
    humk=data['Hum'][-1]
    fig = plotly.subplots.make_subplots(rows=7, cols=1)
    fig['layout']['margin'] = {
        'l': 20, 'r': 20, 'b': 20, 't':20
    }
   # fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}

    fig.append_trace({
        'x': data['Time'],
        'y': data['Temp'],
        'name': 'Temperature-Time',
        'mode': 'lines+markers',
    }, 1, 1)
    fig.append_trace({
        'x': data['Time'],
        'y': data['Hum'],
        'name': 'Humidity-Time',
        'mode': 'lines+markers',
    }, 2, 1)
    fig.append_trace({
        'x': data['Time'],
        'y': data['EM1'],
        'name': 'EM1(Enegy)-Time',
        'mode': 'lines+markers',
    }, 3, 1)
    fig.append_trace({
        'x': data['Time'],
        'y': data['EM2'],
        'name': 'EM2(Energy)-Time',
        'mode': 'lines+markers',
    }, 3, 1)
    fig.append_trace({
        'x': data['Time'],
        'y': data['EM1p'],
        'name': 'EM1(Power)-Time',
        'mode': 'lines+markers',
    }, 4, 1)
    fig.append_trace({
        'x': data['Time'],
        'y': data['EM2p'],
        'name': 'EM2(Power)-Time',
        'mode': 'lines+markers',
    }, 4, 1)
    fig.append_trace({
        'x': data['Time'],
        'y': data['EnergyDiff'],
        'name': 'Energy Difference - Time',
        'mode': 'lines+markers',
    }, 5, 1)
    fig.append_trace({
        'x': data['EM1'],
        'y': data['EM2'],
        'name': 'Energy(EM2)-Energy(EM1)',
        'mode': 'lines+markers',
    }, 6, 1)
    fig.append_trace({
        'x': data['Time'],
        'y': data['Efficiency'],
        'name': 'Efficiency - Time',
        'mode': 'lines+markers',
    }, 7, 1)

    fig.update_layout(
    width=1900,
    height=3000,
    paper_bgcolor="LightSteelBlue",
    uirevision = True
    )

    return fig

if __name__ == '__main__':
    app.run_server()
